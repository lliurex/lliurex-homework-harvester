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

#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#include <vector>
#include <string>

using namespace std;

/*
    Returns a list of group names that current user belongs to
    In case of error it just returns an empty list
*/
static vector<string> getUserGroups()
{
    vector<string> ret;
    
    /* get current user id */
    uid_t user=getuid();
    
    struct passwd* pass = getpwuid(user);
    
    /* hardcoded to ¿only? 32 groups */
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