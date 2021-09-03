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

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QQuickView>
#include <QQmlContext>
#include <QDebug>

#include <iostream>

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
    parser.addPositionalArgument("action", "action to do (add | del | manage)");
    parser.addPositionalArgument("target", "folder to share");
    
    parser.process(app);
    
    QString action;
    QString target;
    
    const QStringList args = parser.positionalArguments();
    
    if (args.size()<1) {
        parser.showHelp(0);
    }
    
    if (args[0]!="add") {
         if (args[0]!="del") {
              if (args[0]!="manage") {
                cerr<<"Error: expected action: add, del or manage, not "<<args[0].toStdString()<<endl;
                return 1;
              }
         }
    }
    
    action = args[0];
    
    if (args[0]=="add" or args[0]=="del") {
        if (args.size()<2) {
            cerr<<"Error: missing target folder"<<endl;
            return 2;
        }
        
        target = args[1];
        
        QFileInfo info(target);
        
        if (!info.exists() or !info.isDir()) {
            cerr<<"Error: target "<<target.toStdString()<<" is not a folder"<<endl;
            return 3;
        }
        else {
            clog<<"folder:"<<target.toStdString()<<endl;
            target=info.absoluteFilePath();
            clog<<"folder:"<<target.toStdString()<<endl;
        }
    }
    
    QQuickView *view = new QQuickView;
    QQmlContext* ctxt = view->rootContext();
    ctxt->setContextProperty("teacherAction",action);
    ctxt->setContextProperty("teacherTarget",target);
    view->setSource(QUrl(QStringLiteral("qrc:/teacher.qml")));
    
    view->show();
    
    return app.exec();

}
