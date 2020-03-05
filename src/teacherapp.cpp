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

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <iostream>

using namespace harvester;
using namespace std;

int main(int argc,char* argv[])
{
    
    QApplication app(argc,argv);
    QApplication::setApplicationName("llx-homework-teacher");
    QApplication::setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("LliureX Homework Harvester teacher gui");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("action", "action to do (add | del)");
    parser.addPositionalArgument("destination", "folder to share");
    
    parser.process(app);
    
    const QStringList args = parser.positionalArguments();
    
    if (args.size()<2) {
        parser.showHelp(0);
    }
    
    TeacherAction action;
    
    if (args[0]=="add") {
       action=TeacherAction::Add;
    }
    else {
        if (args[0]=="del") {
            action=TeacherAction::Delete;
        }
        else {
            cerr<<"Error: expected action: add or del, not "<<args[0].toStdString()<<endl;
            
            return 1;
        }
    }
    TeacherWindow win(action,args[1]);
    
    app.exec();
    
    return 0;
}