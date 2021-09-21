/*
 * Copyright (C) 2021 Lliurex project
 *
 * Author:
 *  Enrique Medina Gremaldos <quiqueiii@gmail.com>
 *
 * Source:
 *  https://github.com/lliurex/lliurex-homework-harvester
 *
 * This file is a part of lliurex homework harvester
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

import Edupals.N4D 1.0 as N4D

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.16 as Kirigami

import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0
import QtQml.Models 2.1

QQC2.Pane {
    id: mainPane
    
    visible:true
    width: 400
    height: 600
    anchors.centerIn:parent
    
    function insertFile(path) {
        
        for (var n=0;n<modelFiles.count;n++) {
            var f=modelFiles.get(n);
            
            if (f.path==path) {
                console.log("File already exists on model");
                return;
            }
        }
        
        var name = path.split('/').reverse()[0];
        modelFiles.append({"name":name,"path":path});
    }
    
    N4D.Client {
        id: n4d
        address: "https://server:9779"
        credential: N4D.Client.Anonymous
        
    }
    
    N4D.Client {
        id: n4dLocal
        address: "https://localhost:9779"
        credential: N4D.Client.Anonymous
        
    }
    
    N4D.Proxy {
        id: share_get_paths
        client: n4d
        plugin: "TeacherShareManager"
        method:"get_paths"
        
        onResponse: {
            for (var key in value) {
                console.log(key,":",value[key]);
                modelTeachers.append({"name":key});
            }
        }
        
        onError: {
            console.log("n4d error:\n",what);
            msg.text=i18nd("lliurex-homework-harvester","Can not list shares");
            msg.visible=true;
        }
    }
    
    N4D.Proxy {
        id: send_to_teacher
        client: n4dLocal
        plugin: "TeacherShare"
        method:"send_to_teacher_net"
        
        onResponse: {
            queue.remove(0);
            if (queue.count>0) {
                lblProgress.text="Sending files "+queue.count+" from "+modelFiles.count;
                pbar.value = 1.0-(queue.count/modelFiles.count);
                
                console.log("sending:",queue.get(0).name);
                send_to_teacher.call([userName,modelTeachers.get(modelTeachers.currentIndex).name,queue.get(0).path]);
            }
            else {
                msg.type=Kirigami.MessageType.Positive;
                msg.text=i18nd("lliurex-homework-harvester","Files sent!");
                msg.visible=true;
                progress.visible=false;
            }
        }
        
        onError: {
            console.log("n4d error:\n",what);
            msg.type=Kirigami.MessageType.Error;
            msg.text=i18nd("lliurex-homework-harvester","Error sending files");
            msg.visible=true;
            progress.visible=false;
        }
    }
    
    Component.onCompleted: {
        
        for (var file in files) {
            insertFile(files[file]);
        }
        
        share_get_paths.call([]);
    }
    
    FileDialog {
        id: fileDialog
        title: i18nd("lliurex-homework-harvester","Select files");
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            for (var n=0;n<fileDialog.fileUrls.length;n++) {
                insertFile(fileDialog.fileUrls[n]);
            }
        }
        onRejected: {
            console.log("Canceled")
            //Qt.exit(0)
        }
        
    }
    
    ListModel {
        id: modelFiles
    }
    
    ListModel {
        id: modelTeachers
    }
    
    ListModel {
        id: queue
    }
    

    ColumnLayout {
        anchors.fill:parent
        
        QQC2.Label {
            text: i18nd("lliurex-homework-harvester","Files")
        }
        
        ListView {
            id: listFiles
            //Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 250
            Layout.preferredHeight: 300
            //Layout.fillHeight: true
            
            model:modelFiles
            highlightFollowsCurrentItem: true
            
            delegate: Kirigami.BasicListItem {
                label: model.name
            }
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignLeft
            Layout.fillWidth: true
            
            QQC2.Button {
                text:"+"
                
                onClicked: {
                    fileDialog.open();
                }
            }
            
            QQC2.Button {
                text:"-"
                
                onClicked: {
                    modelFiles.remove(listFiles.currentIndex);
                }
            }
        }
        
        QQC2.Label {
            text: i18nd("lliurex-homework-harvester","Teacher")
        }
        
        ListView {
            //Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 250
            Layout.preferredHeight: 100
            
            model:modelTeachers
            highlightFollowsCurrentItem: true
            
            delegate: Kirigami.BasicListItem {
                label: model.name
            }
        }
        
        ColumnLayout {
            id: progress
            visible:false;
            
            Layout.fillWidth: true
            
            QQC2.ProgressBar {
                id: pbar
                Layout.fillWidth: true
                
            
            }
            
            QQC2.Label {
                id: lblProgress
                Layout.fillWidth: true
                
            }
        }
        
        Kirigami.InlineMessage {
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth:true
            Layout.minimumHeight:32
            
            id: msg
            //type: Kirigami.MessageType.Error
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            
            QQC2.Button {
                id: btnSend
                text:i18nd("lliurex-homework-harvester","Send")
                
                onClicked: {
                    
                    queue.clear();
                    
                    for (var n=0;n<modelFiles.count;n++) {
                        console.log(modelFiles.get(n).name);
                        queue.append(modelFiles.get(n));
                    }
                    
                    console.log("send to ",modelTeachers.get(modelTeachers.currentIndex).name);
                    
                    msg.visible=false;
                    btnSend.enabled=false;
                    progress.visible=true;
                    lblProgress.text="Sending files "+queue.count+" from "+modelFiles.count;
                    
                    console.log("sending:",queue.get(0).name);
                    send_to_teacher.call([userName,modelTeachers.get(modelTeachers.currentIndex).name,queue.get(0).path]);
                }
                
            }
            
            QQC2.Button {
                text:i18nd("lliurex-homework-harvester","Cancel")
                
                onClicked: {
                    Qt.exit(0);
                }
            }
        }
        
    }
}
