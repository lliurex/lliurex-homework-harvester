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
#include <QList>
#include <QVariant>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDebug>

#include <iostream>

using namespace edupals;
using namespace std;

int main(int argc,char* argv[])
{
    
    QApplication app(argc,argv);
    QApplication::setApplicationName("llx-homework-student");
    QApplication::setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("LliureX Homework Harvester student gui");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", "files to share");
    
    parser.process(app);
    
    QQuickView *view = new QQuickView;
    view->setIcon(QIcon::fromTheme("lliurex-homework-harvester"));
    view->setMinimumSize(QSize(400,600));
    view->setMaximumSize(QSize(400,600));
    
    QQmlContext* ctxt = view->rootContext();
    
    const QStringList files = parser.positionalArguments();
    
    clog<<"Files:"<<endl;
    QList<QVariant> absFiles;
    
    for (QString name:files) {
        QFileInfo info(name);
        clog<<info.absoluteFilePath().toStdString()<<endl;
        absFiles.append(info.absoluteFilePath());
    }
    
    ctxt->setContextProperty("files",absFiles);
    
    system::User me = system::User::me();
    ctxt->setContextProperty("userName",QString::fromStdString(me.name));
    QObject::connect(ctxt->engine(),&QQmlEngine::exit,&app,&QCoreApplication::exit);
    view->setSource(QUrl(QStringLiteral("qrc:/student.qml")));
    view->show();
    
    return app.exec();
}
