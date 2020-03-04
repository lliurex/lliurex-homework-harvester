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

#include <iostream>

using namespace harvester;
using namespace std;

TeacherWindow::TeacherWindow() : QMainWindow()
{
    setWindowTitle("Receive homework");
    setWindowIcon(QIcon::fromTheme("folder-public"));
    setFixedSize(QSize(400, 550));
    setWindowFlags(Qt::Dialog);
    
    QFrame* mainFrame;
    QVBoxLayout* mainLayout;
    
    mainFrame = new QFrame();
    mainLayout = new QVBoxLayout();
    mainFrame->setLayout(mainLayout);
    setCentralWidget(mainFrame);
    
    QDialogButtonBox* buttonBox;
    
    buttonBox = new QDialogButtonBox();
    QAbstractButton* btnClose;
    QAbstractButton* btnAdd;
    QAbstractButton* btnDel;
    btnClose=buttonBox->addButton(QDialogButtonBox::Close);
    
    btnAdd=buttonBox->addButton("Add",QDialogButtonBox::ActionRole);
    
    connect(buttonBox,&QDialogButtonBox::clicked, [this](QAbstractButton* button) {
        this->close();
    });
    
    mainLayout->addWidget(buttonBox);
    
    show();
}