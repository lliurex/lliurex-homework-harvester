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

#include "teacherwindow.hpp"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QStringRef>
#include <QFile>
#include <QFileInfo>
#include <QLineEdit>
#include <QDebug>
#include <QTimer>

#include <iostream>
#include <thread>
#include <future>

using namespace harvester;
using namespace edupals;
using namespace edupals::variant;
using namespace edupals::n4d::agent;
using namespace std;

teacher::Window::Window(Action action,QString destination) : QMainWindow()
{
    m_action=action;
    m_step=Step::None;
    
    //TODO:Must point to server
    m_n4d = n4d::Client("https://localhost",9779);

    Variant ret = m_n4d.call("TeacherShareManager","get_paths");
    clog<<ret<<endl;
    
    QFileInfo info(destination);
    
    qDebug()<<"1 "<<destination;
    m_destination=info.canonicalFilePath();
    qDebug()<<"2 "<<m_destination;
    
    m_name = m_destination.splitRef("/").last().toString();
    //m_name=info.fileName();
    qDebug()<<"3 "<<m_name;
    
    setWindowTitle("Receive homework");
    setWindowIcon(QIcon::fromTheme("folder-public"));
    setFixedSize(QSize(400, 300));
    setWindowFlags(Qt::Dialog);
    
    QStackedWidget* stackFrame = new QStackedWidget();
    setCentralWidget(stackFrame);
    storage["stack"]=stackFrame;
    
    QFrame* mainFrame;
    QVBoxLayout* mainLayout;
    
    QString actionName;
    QString descriptionMessage;
    
    if (m_action==teacher::Action::Add) {
        actionName="Add";
        descriptionMessage="Add this folder to receive homeworks";
    }
    if (m_action==teacher::Action::Delete) {
        actionName="Delete";
        descriptionMessage="Remove this folder as homework destinations";
    }
    
    mainFrame = new QFrame();
    mainLayout = new QVBoxLayout();
    mainFrame->setLayout(mainLayout);
    stackFrame->addWidget(mainFrame);
    
    QLabel* message = new QLabel(descriptionMessage);
    message->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(message);
    
    QLabel* lblIcon = new QLabel();
    lblIcon->setAlignment(Qt::AlignCenter);
    QIcon icon=QIcon::fromTheme("folder-network");
    lblIcon->setPixmap(icon.pixmap(64,64));
    mainLayout->addWidget(lblIcon);
    
    if (m_action==teacher::Action::Add) {
        QStackedWidget* stack=new QStackedWidget();
        QPushButton* btnName = new QPushButton(m_name);
        btnName->setFlat(true);
        
        connect(btnName,&QPushButton::clicked, [stack]() mutable {
            stack->setCurrentIndex(1);
        });
        
        stack->addWidget(btnName);
        
        QLineEdit* txtName = new QLineEdit(m_name);
        txtName->setAlignment(Qt::AlignCenter);
        connect(txtName,&QLineEdit::editingFinished, [stack,btnName,txtName]() mutable {
            btnName->setText(txtName->text());
            stack->setCurrentIndex(0);
        });
        
        stack->addWidget(txtName);
        
        mainLayout->addWidget(stack);
    }
    
    QLabel* lblFileName = new QLabel(m_destination);
    lblFileName->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblFileName);
    
    mainLayout->addStretch(1);
    
    QDialogButtonBox* buttonBox;
    
    buttonBox = new QDialogButtonBox();
    QAbstractButton* btnClose;
    QAbstractButton* btnAction;
    btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    
    btnAction=buttonBox->addButton(actionName,QDialogButtonBox::ActionRole);
    
    storage["btnAction"]=btnAction;
    storage["btnClose"]=btnClose;
    
    connect(buttonBox,&QDialogButtonBox::clicked, this, &teacher::Window::buttonBoxClicked);
    
    mainLayout->addWidget(buttonBox);
    
    btnAction->setFocus();
    
    // secondary Frame
    QFrame* secondaryFrame;
    QVBoxLayout* secondaryLayout;
    
    secondaryFrame = new QFrame();
    secondaryLayout = new QVBoxLayout();
    secondaryFrame->setLayout(secondaryLayout);
    
    stackFrame->addWidget(secondaryFrame);
    
    QLabel* lblStatus = new QLabel("In progres...");
    lblStatus->setAlignment(Qt::AlignCenter);
    secondaryLayout->addWidget(lblStatus);
    
    buttonBox = new QDialogButtonBox();
    btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    connect(buttonBox,&QDialogButtonBox::clicked, this, [this](){
        this->close();
    });
    secondaryLayout->addWidget(buttonBox);
    
    timer=new QTimer();
    storage["timer"]=timer;
    connect(timer, &QTimer::timeout, this, &teacher::Window::timeout);
    timer->start(500);
    
    show();
}

teacher::Window::~Window()
{
    delete timer;
}

int teacher::Window::performN4D(teacher::Task task)
{
    getIP();
    
    n4d::Client client(task.ticket.address,
                    task.ticket.port,
                    task.ticket.credential.user,
                    task.ticket.credential.key
    );
    
    variant::Variant ret;
    
    switch (task.action) {
        case Action::Add:
            ret = client.call("TeacherShareManager","add_path",
                              { task.ticket.credential.user,
                task.destination,
                task.name }
            );
            
            clog<<ret<<endl;
        break;
        
        case Action::Delete:
        break;
    }
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    
    return 0;
}

string teacher::Window::getIP()
{
    /* localhost client */
    n4d::Client client;
    variant::Variant ret;
    
    ret = client.call("VariablesManager","get_variable",{"CLIENT_INTERNAL_INTERFACE"});
    
    if (ret.none()) {
        ret = client.call("VariablesManager","get_variable",{"INTERNAL_INTERFACE"});
    }
    
    clog<<"iface:"<<ret<<endl;
    
    ret = client.rpc_call("get_ip",{"wlan0"});
    
    clog<<"ip: "<<ret<<endl;
    
    return "";
}

void teacher::Window::timeout()
{
    switch (m_step) {
        case Step::WaitLogin:
            if (login->ready()) {
                
                m_step = Step::None;
                m_ticket = login->value();
                
                if (m_ticket.valid()) {
                    m_step = Step::WaitN4DCall;
                    
                    teacher::Task task;
                    task.action=m_action;
                    task.ticket=m_ticket;
                    task.destination=m_destination.toStdString();
                    task.name=m_name.toStdString();
                    m_ret = std::async(&teacher::Window::performN4D,this,task);
                    
                    static_cast<QStackedWidget*>(storage["stack"])->setCurrentIndex(1);

                }
                else {
                    static_cast<QAbstractButton*>(storage["btnAction"])->setEnabled(true);
                }
                
                delete login;
            }
        break;
        
        case Step::WaitN4DCall:
            
            if (m_ret.wait_for(std::chrono::milliseconds(50))==std::future_status::ready) {
                clog<<"Cal ret:"<<m_ret.get()<<endl;
                m_step=Step::None;
            }
        break;
    }
}

void teacher::Window::buttonBoxClicked(QAbstractButton* button)
{
    if (button==storage["btnClose"]) {
        this->close();
    }
    
    if (button==storage["btnAction"]) {
        button->setEnabled(false);
        
        login = new LoginDialog();
        login->run();
        m_step = Step::WaitLogin;
        //std::async(std::launch::deferred,&teacher::Window::waitLogin,this);
        //worker = std::thread(&teacher::Window::waitLogin,this);
    }
    
}
