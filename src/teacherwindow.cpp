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

#include <iostream>

using namespace harvester;
using namespace std;

TeacherWindow::TeacherWindow(TeacherAction action,QString destination) : QMainWindow()
{
    m_action=action;
    
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
    
    QFrame* mainFrame;
    QVBoxLayout* mainLayout;
    
    QString actionName;
    QString descriptionMessage;
    
    if (m_action==TeacherAction::Add) {
        actionName="Add";
        descriptionMessage="Add this folder to receive homeworks";
    }
    if (m_action==TeacherAction::Delete) {
        actionName="Delete";
        descriptionMessage="Remove this folder as homework destinations";
    }
    
    mainFrame = new QFrame();
    mainLayout = new QVBoxLayout();
    mainFrame->setLayout(mainLayout);
    setCentralWidget(mainFrame);
    
    QLabel* message = new QLabel(descriptionMessage);
    message->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(message);
    
    QLabel* lblIcon = new QLabel();
    lblIcon->setAlignment(Qt::AlignCenter);
    QIcon icon=QIcon::fromTheme("folder-network");
    lblIcon->setPixmap(icon.pixmap(64,64));
    mainLayout->addWidget(lblIcon);
    
    if (m_action==TeacherAction::Add) {
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
    QAbstractButton* btnDel;
    btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    
    btnAction=buttonBox->addButton(actionName,QDialogButtonBox::ActionRole);
    
    connect(buttonBox,&QDialogButtonBox::clicked, [this](QAbstractButton* button) {
        this->close();
    });
    
    mainLayout->addWidget(buttonBox);
    
    show();
}