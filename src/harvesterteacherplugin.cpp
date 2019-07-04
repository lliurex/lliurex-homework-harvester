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

#include "harvesterteacherplugin.hpp"
#include "locale.hpp"
#include "util.hpp"

#include <KFileItem>
#include <QString>
#include <QProcess>

#include <iostream>

using namespace std;
using namespace lliurex::locale;

HarvesterTeacherPlugin::HarvesterTeacherPlugin(QObject* parent, const QVariantList& list) : KAbstractFileItemActionPlugin(parent)
{
    lliurex::locale::domain("lliurex-homework-harvester-plugin");
    
    icon=QIcon::fromTheme("download");
    actionReceive=new QAction(this);
    actionReceive->setText(T("Receive homework here"));
    actionReceive->setIcon(icon);
    
    connect(actionReceive,&QAction::triggered,this,&HarvesterTeacherPlugin::triggered);
}

HarvesterTeacherPlugin::~HarvesterTeacherPlugin()
{
}

void HarvesterTeacherPlugin::triggered(bool checked)
{
    
    QProcess child;
    
    child.setProgram("/usr/bin/lliurex-homework-harvester");
    child.setArguments({target.toLocalFile()});
    child.startDetached(nullptr);
    
}

QList<QAction* > HarvesterTeacherPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> list;
    
    KFileItemList files = fileItemInfos.items();
    
    /* we are not interested in multiple selections */
    if (files.size()>1) {
        return list;
    }
    
    vector<string> groups = getUserGroups();
    
    bool isTeacher=false;
    
    for (string group : groups) {
        if (group=="teachers") {
            isTeacher=true;
            break;
        }
    }
    
    if (isTeacher and fileItemInfos.isDirectory() and fileItemInfos.supportsWriting()) {
        list.append(actionReceive);

        target = files.first().targetUrl();
    }
    
    return list;
}

K_PLUGIN_FACTORY_WITH_JSON(HarvesterTeacherPluginFactory, "harvesterteacherplugin.json", registerPlugin<HarvesterTeacherPlugin>();)
#include <harvesterteacherplugin.moc>