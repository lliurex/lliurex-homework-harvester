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

#ifndef HARVESTER_STUDENT_WINDOW
#define HARVESTER_STUDENT_WINDOW

#include <QMainWindow>
#include <QStringList>

#include <string>
#include <map>

namespace harvester
{
    class StudentWindow: public QMainWindow
    {
        Q_OBJECT
        
        public:
        
        std::map<std::string,QObject*> storage;
        
        StudentWindow(QStringList files);
        
    };
}

#endif