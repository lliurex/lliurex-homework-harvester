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

#include <n4d.hpp>

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
using namespace edupals;
using namespace std;

student::Window::Window(QStringList files) : QMainWindow()
{
    m_step = Step::Load;
    m_ret = std::async(&student::Window::loadPaths,this);
    
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
    storage["stack"]=stack;
    mainLayout->addWidget(stack);
    
    //frame 1
    Ui::frame1* frame1 = new Ui::frame1();
    QFrame* container = new QFrame();
    frame1->setupUi(container);
    stack->addWidget(container);
    
    storage["listTeachers"]=frame1->listTeachers;
    
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
    

    QFrame* frame2 = new QFrame();
    QVBoxLayout*vlayout = new QVBoxLayout();
    frame2->setLayout(vlayout);
    QLabel* lbl;
    lbl=new QLabel("Loading...");
    
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    QPixmap px("://wait00.svg");
    lbl = new QLabel();
    
    storage["frame2.image"]=lbl;
    lbl->setPixmap(px);
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    
    vlayout->addStretch(1);
    
    stack->addWidget(frame2);
    
    QTimer *timer = new QTimer(this);
    storage["timer"]=timer;
    timer->start(500);
    connect(timer, &QTimer::timeout, this, &student::Window::pulse);
    
    //Frame 3
    QFrame* frame3 = new QFrame();
    vlayout = new QVBoxLayout();
    frame3->setLayout(vlayout);
    lbl=new QLabel("Done!");
    
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    stack->addWidget(frame3);
    
    //ButtonBox
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
            stack->setCurrentIndex(1);
            m_step=Step::Send;
            btnSend->hide();
        }
    });
    
    mainLayout->addWidget(buttonBox);
    stack->setCurrentIndex(1);
    
    show();
}

int student::Window::loadPaths()
{
    n4d::Client client;
    variant::Variant ret;
    
    ret=client.call("TeacherShareManager","get_paths");
    
    if (ret.type()==variant::Type::Struct) {
        vector<string> keys = ret.keys();
        
        //Better in a mutex!
        this->paths.clear();
        
        for (auto k:keys) {
            this->paths.push_back(k);
        }
    }
    else {
        return 1;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    
    return 0;
}

void student::Window::pulse()
{
    
    QLabel* label;
    static int imgIndex=0;
    static const QString images[4]={"://wait00.svg","://wait01.svg","://wait02.svg","://wait03.svg"};
    
    static QPixmap px;
    
    QListWidget* list;
    
    switch (m_step) {
        case Step::Load:
            if (m_ret.wait_for(chrono::milliseconds(50))==std::future_status::ready) {
                m_step=Step::None;
                static_cast<QStackedWidget*>(storage["stack"])->setCurrentIndex(0);
                list = static_cast<QListWidget*>(storage["listTeachers"]);
                
                list->reset();
                if (m_ret.get()==0) {
                    
                    for (auto p: paths) {
                        list->addItem(QString::fromStdString(p));
                    }
                }
            }
        break;
        
        case Step::Send:
            imgIndex++;
            imgIndex=imgIndex%4;
            label = static_cast<QLabel*>(storage["frame2.image"]);
            px=QPixmap(images[imgIndex]);
            label->setPixmap(px);
        break;
    }
}
