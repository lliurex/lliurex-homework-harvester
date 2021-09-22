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

#include <user.hpp>

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QDebug>

#include <iostream>

using namespace std;
using namespace edupals;

int main(int argc,char* argv[])
{
    
    QApplication app(argc,argv);
    QApplication::setApplicationName("llx-homework-teacher");
    QApplication::setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("LliureX Homework Harvester teacher gui");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("target", "folder to share");
    
    parser.process(app);
    
    QString target;
    
    const QStringList args = parser.positionalArguments();
    
    if (args.size()<1) {
        parser.showHelp(0);
    }
    
    target = args[0];
    
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
    
    QQuickView *view = new QQuickView;
    view->setMinimumSize(QSize(400,360));
    view->setMaximumSize(QSize(400,360));
    
    QQmlContext* ctxt = view->rootContext();
    QObject::connect(ctxt->engine(),&QQmlEngine::exit,&app,&QCoreApplication::exit);
    ctxt->setContextProperty("teacherTarget",target);
    
    system::User me = system::User::me();
    ctxt->setContextProperty("userName",QString::fromStdString(me.name));
    
    view->setSource(QUrl(QStringLiteral("qrc:/teacher.qml")));
    
    view->show();
    
    // center on screen
    QScreen* screen = view->screen();
    uint32_t x = screen->geometry().width()/2 - view->frameGeometry().width()/2;
    uint32_t y = screen->geometry().height()/2 - view->frameGeometry().height()/2;
    view->setPosition(x,y);
    
    return app.exec();
}
