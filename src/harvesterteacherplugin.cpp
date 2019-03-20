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
 
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
 
#include <iostream>
#include <vector>
#include <string>

using namespace std;

/*
    Returns a list of group names that current user belongs to
    In case of error it just returns an empty list
*/
vector<string> getUserGroups()
{
    vector<string> ret;
    
    /* get current user id */
    uid_t user=getuid();
    
    struct passwd* pass = getpwuid(user);
    gid_t groups[32];
    int ngroups=32;
    
    if (pass==nullptr) {
        return ret;
    }
    else {
        int status=getgrouplist(pass->pw_name,pass->pw_gid,groups,&ngroups);
        
        if (status==-1) {
            return ret;
        }
        else {
            for (int n=0;n<ngroups;n++) {
                struct group* grp=getgrgid(groups[n]);
                if (grp!=nullptr) {
                    ret.push_back(string(grp->gr_name));
                }
            }
        }
    }
    
    return ret;
}

HarvesterTeacherPlugin::HarvesterTeacherPlugin(QObject* parent, const QVariantList& list) : KAbstractFileItemActionPlugin(parent)
{
    actionReceive=new QAction(this);
    actionReceive->setText(T("Receive homework here"));
    
    connect(actionReceive,&QAction::triggered,this,&HarvesterTeacherPlugin::triggered);
}

HarvesterTeacherPlugin::~HarvesterTeacherPlugin()
{
}

void HarvesterTeacherPlugin::triggered(bool checked)
{
    clog<<"Triggered "<<checked<<endl;
}

QList<QAction* > HarvesterTeacherPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> list;
    
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
    }
    
    return list;
}

K_PLUGIN_FACTORY_WITH_JSON(HarvesterTeacherPluginFactory, "harvesterteacherplugin.json", registerPlugin<HarvesterTeacherPlugin>();)
#include <harvesterteacherplugin.moc>