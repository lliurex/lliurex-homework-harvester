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
 
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
 
#include <iostream>

using namespace std;

HarvesterTeacherPlugin::HarvesterTeacherPlugin(QObject* parent, const QVariantList& list) : KAbstractFileItemActionPlugin(parent)
{
    clog<<"plugin created"<<endl;
    
    /* get current user id */
    uid_t user=getuid();
    
    struct passwd* pass = getpwuid(user);
    gid_t groups[32];
    int ngroups=32;
    
    if (pass==nullptr) {
        cerr<<"Error retrieving user info"<<endl;
    }
    else {
        
        int status=getgrouplist(pass->pw_name,pass->pw_gid,groups,&ngroups);
        
        if (status==-1) {
            
        }
        else {
            for (int n=0;n<ngroups;n++) {
                struct group* grp=getgrgid(groups[n]);
                if (grp!=nullptr) {
                    clog<<"* "<<grp->gr_name<<endl;
                }
            }
        }
        
    }
    
    actionReceive=new QAction(this);
    actionReceive->setText("Receive homework here");
}

HarvesterTeacherPlugin::~HarvesterTeacherPlugin()
{
    clog<<"plugin destroyed!"<<endl;
}

QList<QAction* > HarvesterTeacherPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> list;
    
    list.append(actionReceive);
    
    return list;
}

K_PLUGIN_FACTORY_WITH_JSON(HarvesterTeacherPluginFactory, "harvesterteacherplugin.json", registerPlugin<HarvesterTeacherPlugin>();)
#include <harvesterteacherplugin.moc>