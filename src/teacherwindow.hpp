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

#ifndef HARVESTER_TEACHER_WINDOW
#define HARVESTER_TEACHER_WINDOW

#include <agent.hpp>

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QTimer>

#include <string>
#include <map>
#include <future>

namespace harvester
{
    namespace teacher
    {
        enum class Action
        {
            Add,
            Delete
        };
        
        enum class Step
        {
            None,
            WaitLogin,
            WaitN4DCall
        };
        
        struct Task
        {
            Action action;
            edupals::n4d::agent::Ticket ticket;
            std::string destination;
            std::string name;
        };
        
        class Window: public QMainWindow
        {
            Q_OBJECT
            
            public:
            
            edupals::n4d::Client m_n4d;
            edupals::n4d::agent::Ticket m_ticket;

            std::future<int> m_ret;
            
            edupals::n4d::agent::LoginDialog* login;
            
            std::map<std::string,QObject*> storage;
            
            Action m_action;
            Step m_step;
            
            QString m_destination;
            QString m_name;
            
            QTimer* timer;
            
            Window(Action action,QString destination);
            ~Window();
            
            /*
             * Asynchronous execution of n4d tasks
             */
            int performN4D(Task task);
            
            std::string getIP();
            
            void pulse();
            void buttonBoxClicked(QAbstractButton* button);
        };
    }
}

#endif
