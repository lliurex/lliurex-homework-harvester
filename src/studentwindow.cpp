/*
    Lliurex homework harvester

    Copyright (C) 2020  Enrique Medina Gremaldos <quiqueiii@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "studentwindow.hpp"
#include "ui_studentframe1.h"

#include <QFileDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QImage>
#include <QDebug>
#include <QTimer>

#include <iostream>

using namespace harvester;
using namespace std;

student::Window::Window(QStringList files) : QMainWindow()
{
    setWindowTitle("Send files to teacher");
    setWindowIcon(QIcon::fromTheme("folder-public"));
    setFixedSize(QSize(400, 550));
    setWindowFlags(Qt::Dialog);
    
    QFrame* mainFrame;
    QVBoxLayout* mainLayout;
    
    mainFrame = new QFrame();
    mainLayout = new QVBoxLayout();
    mainFrame->setLayout(mainLayout);
    setCentralWidget(mainFrame);
    
    QStackedWidget* stack=new QStackedWidget();
    mainLayout->addWidget(stack);
    
    //frame 1
    Ui::frame1* frame1 = new Ui::frame1();
    QFrame* container = new QFrame();
    frame1->setupUi(container);
    stack->addWidget(container);
    
    for (auto f:files) {
        frame1->listFiles->addItem(f);
    }
    connect(frame1->btnAdd,&QPushButton::clicked, [this,frame1]() mutable {
        QStringList files = QFileDialog::getOpenFileNames(this,"Add files");
        
        for (auto f:files) {
            clog<<"* "<<f.toStdString()<<endl;
            frame1->listFiles->addItem(f);
        }
    });
    
    connect(frame1->btnRemove,&QPushButton::clicked, [frame1]() mutable {
        int row = frame1->listFiles->currentRow();
        
        if (row>=0) {
            delete(frame1->listFiles->takeItem(row));
        }
    });
    
    //frame 2
    int imgId=0;
    const QString images[4]={"://wait00.svg","://wait01.svg","://wait02.svg","://wait03.svg"};

    QFrame* frame2 = new QFrame();
    QVBoxLayout*vlayout = new QVBoxLayout();
    frame2->setLayout(vlayout);
    QLabel* lbl;
    lbl=new QLabel("Sending files...");
    
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    QPixmap px(images[0]);
    lbl = new QLabel();
    
    storage["frame2.image"]=lbl;
    lbl->setPixmap(px);
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    
    vlayout->addStretch(1);
    
    stack->addWidget(frame2);
    
    QTimer *timer = new QTimer(this);
    storage["frame2.timer"]=timer;
    
    connect(timer,&QTimer::timeout,[=]()mutable {
        imgId++;
        imgId=imgId%4;
        QPixmap px(images[imgId]);
        lbl->setPixmap(px);
    });
    
    QDialogButtonBox* buttonBox;
    buttonBox = new QDialogButtonBox();
    QAbstractButton* btnClose;
    QAbstractButton* btnSend;
    btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    btnSend=buttonBox->addButton("send",QDialogButtonBox::ActionRole);
    
    connect(buttonBox,&QDialogButtonBox::clicked, [=](QAbstractButton* button) {
        if (button==btnClose) {
            this->close();
        }
        
        if (button==btnSend) {
            qDebug()<<"sending files...";
            stack->setCurrentIndex(1);
            timer->start(500);
            btnSend->hide();
        }
    });
    
    mainLayout->addWidget(buttonBox);
    
    show();
}
