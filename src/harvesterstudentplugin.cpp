/*
    Lliurex homework harvester

    Copyright (C) 2019  Enrique Medina Gremaldos <quiqueiii@gmail.com>

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

#include "harvesterstudentplugin.hpp"
#include "locale.hpp"
#include "util.hpp"

#include <KFileItem>
#include <QString>
#include <QProcess>

#include <iostream>

using namespace std;
using namespace lliurex::locale;



HarvesterStudentPlugin::HarvesterStudentPlugin(QObject* parent, const QVariantList& list) : KAbstractFileItemActionPlugin(parent)
{
    lliurex::locale::domain("lliurex-homework-harvester-plugin");
    
    actionSend=new QAction(this);
    actionSend->setText(T("Send homework"));
    
    connect(actionSend,&QAction::triggered,this,&HarvesterStudentPlugin::triggered);
}

HarvesterStudentPlugin::~HarvesterStudentPlugin()
{
}

void HarvesterStudentPlugin::triggered(bool checked)
{
    clog<<"student plugin"<<endl;
    for (auto t:target) {
        clog<<"file: "<<t.toLocalFile().toLocal8Bit().data()<<endl;
    }
    /*
    QProcess child;
    
    child.setProgram("/usr/bin/lliurex-homework-harvester");
    child.setArguments({target.toLocalFile()});
    child.start();
    child.waitForFinished();
    */
}

QList<QAction* > HarvesterStudentPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> list;
    
    vector<string> groups = getUserGroups();
    
    bool isStudent=false;
    
    for (string group : groups) {
        if (group=="sudo") {
            isStudent=true;
            break;
        }
    }
    
    if (isStudent) {
        clog<<"Im a student"<<endl;
        KFileItemList files = fileItemInfos.items();
        target.clear();
        
        list.append(actionSend);
        
        for (int n=0;n<files.size();n++) {
            clog<<"pushing..."<<endl;
            KFileItem& item = files[n];
            
            if  (item.isFile()) {
                target.push_back(item.url());
            }
        }
    
    }
    
    return list;
}

K_PLUGIN_FACTORY_WITH_JSON(HarvesterStudentPluginFactory, "harvesterstudentplugin.json", registerPlugin<HarvesterStudentPlugin>();)
#include <harvesterstudentplugin.moc>