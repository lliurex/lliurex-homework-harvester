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

#ifndef HARVESTER_STUDENT_PLUGIN
#define HARVESTER_STUDENT_PLUGIN

#include <KAbstractFileItemActionPlugin>
#include <KFileItemListProperties>
#include <KPluginFactory>
#include <QAction>
#include <QIcon>

class HarvesterStudentPlugin : public KAbstractFileItemActionPlugin
{
    Q_OBJECT
    
    private:
    
    std::vector<QUrl> target;
    QIcon icon;
    QAction* actionSend;
    
    private slots:
    
    void triggered(bool checked = false);
    
    public:
    HarvesterStudentPlugin(QObject* parent, const QVariantList& list);
    ~HarvesterStudentPlugin();
    
    QList<QAction * > actions (const KFileItemListProperties& fileItemInfos, QWidget* parentWidget) override;
 
};

#endif