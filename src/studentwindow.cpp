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
    QApplication::setOverrideCursor(Qt::BusyCursor);
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
    
    m_stack=new QStackedWidget();
    mainLayout->addWidget(m_stack);
    
    //frame 1
    Ui::frame1* frame1 = new Ui::frame1();
    QFrame* container = new QFrame();
    frame1->setupUi(container);
    m_stack->addWidget(container);
    
    m_listTeachers=frame1->listTeachers;
    
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
    lbl = new QLabel();
    
    m_lblImage=lbl;
    lbl->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(lbl);
    
    vlayout->addStretch(1);
    
    m_stack->addWidget(frame2);
    
    m_timer = new QTimer(this);
    m_timer->start(500);
    connect(m_timer, &QTimer::timeout, this, &student::Window::pulse);
    
    //ButtonBox
    QDialogButtonBox* buttonBox;
    buttonBox = new QDialogButtonBox();

    m_btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    m_btnSend=buttonBox->addButton("send",QDialogButtonBox::ActionRole);
    m_btnSend->setEnabled(false);
    
    connect(buttonBox,&QDialogButtonBox::clicked, [=](QAbstractButton* button) {
        if (button==m_btnClose) {
            this->close();
        }
        
        if (button==m_btnSend) {
            m_stack->setCurrentIndex(1);
            m_step=Step::Send;
            m_btnSend->hide();
            
            m_ret = std::async(&student::Window::send,this);
        }
    });
    
    mainLayout->addWidget(buttonBox);
    
    connect(m_listTeachers,&QListWidget::itemSelectionChanged,[=](){
        qDebug()<<m_listTeachers->currentItem()->text();
        m_btnSend->setEnabled(true);
    });
    
    m_stack->setCurrentIndex(1);
    
    show();
}

student::Window::~Window()
{
    delete m_timer;
}

int student::Window::loadPaths()
{
    n4d::Client client;
    variant::Variant ret;
    
    ret=client.call("TeacherShareManager","get_paths");
    
    if (ret.type()==variant::Type::Struct) {
        clog<<ret<<endl;
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
    
    return 0;
}

int student::Window::send()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    return 0;
}

void student::Window::pulse()
{
    
    static int imgIndex=0;
    static const QString images[4]={"://wait00.svg","://wait01.svg","://wait02.svg","://wait03.svg"};
    
    static QPixmap px;
    
    switch (m_step) {
        case Step::Load:
            if (m_ret.wait_for(chrono::milliseconds(50))==std::future_status::ready) {
                m_step=Step::None;
                QApplication::restoreOverrideCursor();
                m_stack->setCurrentIndex(0);
                
                
                m_listTeachers->reset();
                if (m_ret.get()==0) {
                    
                    for (auto p: paths) {
                        m_listTeachers->addItem(QString::fromStdString(p));
                    }
                }
                else {
                    //ToDo: dialog error?
                    cerr<<"Failed to retrieve paths from n4d server"<<endl;
                }
            }
        break;
        
        case Step::Send:
            imgIndex++;
            imgIndex=imgIndex%4;
            px=QPixmap(images[imgIndex]);
            m_lblImage->setPixmap(px);
            
            if (m_ret.wait_for(chrono::milliseconds(50))==std::future_status::ready) {
                if (m_ret.get()==0) {
                    m_step = Step::None;
                    clog<<"done"<<endl;
                }
            }
        break;
    }
}
