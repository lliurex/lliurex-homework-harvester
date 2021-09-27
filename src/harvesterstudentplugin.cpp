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

#include <user.hpp>
#include <group.hpp>

#include <KFileItem>
#include <QString>
#include <QProcess>

#include <iostream>

using namespace std;
using namespace edupals::system;
using namespace lliurex::locale;

HarvesterStudentPlugin::HarvesterStudentPlugin(QObject* parent, const QVariantList& list) : KAbstractFileItemActionPlugin(parent)
{
    lliurex::locale::domain("lliurex-homework-harvester");
    
    icon=QIcon::fromTheme("document-send");
    actionSend=new QAction(this);
    actionSend->setText(T("Send file to teacher"));
    actionSend->setIcon(icon);
    
    connect(actionSend,&QAction::triggered,this,&HarvesterStudentPlugin::triggered);
}

HarvesterStudentPlugin::~HarvesterStudentPlugin()
{
}

void HarvesterStudentPlugin::triggered(bool checked)
{
    /* populate argument list */
    QStringList args;
    
    for (auto t:target) {
        args<<t.toLocalFile();
    }
    
    QProcess child;
    
    child.setProgram("/usr/bin/llx-homework-student");
    child.setArguments(args);
    child.startDetached(nullptr);
    
}

QList<QAction* > HarvesterStudentPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> list;
    
    vector<Group> groups = User::me().groups();
    
    bool isStudent=false;
    
    for (Group& group : groups) {
        if (group.name=="students") {
            isStudent=true;
            break;
        }
    }
    
    if (isStudent) {
        KFileItemList files = fileItemInfos.items();
        target.clear();
        
        list.append(actionSend);
        
        for (int n=0;n<files.size();n++) {
            KFileItem& item = files[n];
            
            if  (item.isFile()) {
                target.push_back(item.targetUrl());
            }
        }
    
    }
    
    return list;
}

K_PLUGIN_FACTORY_WITH_JSON(HarvesterStudentPluginFactory, "harvesterstudentplugin.json", registerPlugin<HarvesterStudentPlugin>();)
#include <harvesterstudentplugin.moc>
