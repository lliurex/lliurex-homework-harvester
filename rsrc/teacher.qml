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
import org.kde.kcoreaddons 1.0 as KCoreAddons

import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0

QQC2.Pane {
    id: main
    
    property string pswd;
    
    visible:true
    width: 400
    height: 360
    anchors.centerIn: parent
    
    KCoreAddons.KUser {
        id: kuser
    }
    
    QQC2.StackView {
        id: stack
        initialItem: loginView
        anchors.fill: parent
    }
    
    Component {
        id: loginView
        
        QQC2.Pane {
            
            N4D.Client {
                id: n4d
                address: "https://server:9779"
                credential: N4D.Client.Password
                user: userName
            }
            
            N4D.Proxy {
                id: validate_auth
                client: n4d
                method:"validate_auth"
                
                onResponse: {
                    passwordField.enabled = true;
                    main.pswd = passwordField.text;
                    passwordField.text = "";
                    stack.push(mainView);
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    
                    if (code==N4D.Error.AuthenticationFailed) {
                        errorLogin.text = i18nd("lliurex-homework-harvester","Authentication failed");
                    }
                    else {
                        errorLogin.text = i18nd("lliurex-homework-harvester","Login failed: %1",code);
                    }
                    errorLogin.visible = true;
                    passwordField.enabled = true;
                    passwordField.text = "";
                }
            }
            
            ColumnLayout {
                anchors.fill:parent
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth:true
                    text:i18nd("lliurex-homework-harvester","Homework harvester needs your teacher password to perform this action")
                    wrapMode:Text.WordWrap
                    
                }
                
                Kirigami.Icon {
                    Layout.alignment: Qt.AlignCenter 
                    Layout.minimumWidth: Kirigami.Units.iconSizes.huge
                    Layout.minimumHeight: width
                    
                    source: kuser.faceIconUrl
                    fallback: "preferences-system-user-sudo"
                    
                }
                
                QQC2.Label {
                    text:userName
                    Layout.alignment: Qt.AlignHCenter
                    
                }
                
                QQC2.TextField {
                    id: passwordField
                    focus: true
                    echoMode: TextInput.Password
                    Layout.alignment: Qt.AlignHCenter
                    
                    onAccepted: {
                        enabled = false;
                        validate_auth.call([[userName,passwordField.text]]);
                    }
                }
                
                Item {
                    height:32
                }
                
                Kirigami.InlineMessage {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth:true
                    Layout.minimumHeight:32
                    
                    id: errorLogin
                    type: Kirigami.MessageType.Error
                }
                
                RowLayout {
                    Layout.alignment: Qt.AlignBottom | Qt.AlignRight

                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Login")
                        
                        onClicked: {
                            passwordField.enabled = false;
                            validate_auth.call([[userName,passwordField.text]]);
                        }
                    }
                    
                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Close")
                        
                        onClicked: {
                            Qt.exit(0);
                        }
                    }
                }
            }
        }
    }
    
    Component {
        id: mainView
        
        QQC2.Pane {
            property string folderName : teacherTarget.split('/').reverse()[0]
            property string teacherAction : "add"
            
            N4D.Client {
                id: n4d
                address: "https://server:9779"
                credential: N4D.Client.Password
            }
            
            N4D.Client {
                id: n4dLocal
                credential: N4D.Client.Password
            }
            
            N4D.Proxy {
                id: share_get_paths
                client: n4d
                plugin:"TeacherShareManager"
                method:"get_paths"
                
                onResponse: {
                    console.log("paths:");
                    for (var key in value) {
                        console.log(key,":",value[key]);
                    }
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                }
            }
            
            N4D.Proxy {
                id: share_is_configured
                client: n4d
                plugin:"TeacherShareManager"
                method:"is_configured"
                
                onResponse: {
                    console.log("is configured:",value);
                    
                    if (value) {
                        teacherAction="del";
                    }
                    else {
                        teacherAction="add";
                    }
                    
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=i18nd("lliurex-homework-harvester","Error checking share status: %1",code);
                    errorLabel.visible=true;
                }
            }
            
            N4D.Proxy {
                id: share_add_path
                client: n4d
                plugin:"TeacherShareManager"
                method:"add_path"
                
                onResponse: {
                    console.log("added:\n",value);
                    
                    errorLabel.type=Kirigami.MessageType.Positive;
                    errorLabel.text=i18nd("lliurex-homework-harvester","Shared directory added");
                    errorLabel.visible=true;
                    
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=i18nd("lliurex-homework-harvester","Unable to add share: %1",code);
                    errorLabel.visible=true;
                }
            }
            
            N4D.Proxy {
                id: share_remove_path
                client: n4d
                plugin:"TeacherShareManager"
                method:"remove_path"
                
                onResponse: {
                    console.log("removed:\n",value);
                    errorLabel.type=Kirigami.MessageType.Positive;
                    errorLabel.text=i18nd("lliurex-homework-harvester","Shared directory removed");
                    errorLabel.visible=true;
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=i18nd("lliurex-homework-harvester","Unable to remove share: %1");
                    errorLabel.visible=true;
                }
            }
            
            N4D.Proxy {
                id: register_share_info
                client: n4dLocal
                plugin:"TeacherShare"
                method:"register_share_info"
                
                onResponse: {
                    console.log("share registered");
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.type=Kirigami.MessageType.Error;
                    errorLabel.text=i18nd("lliurex-homework-harvester","Unable to register share: %1",code);
                    errorLabel.visible=true;
                }
            }
            
            Component.onCompleted: {
                n4d.user = userName;
                n4d.password = main.pswd;
                n4dLocal.user = userName;
                n4dLocal.password = main.pswd;
                
                share_is_configured.call(["",teacherTarget]);
                share_get_paths.call([]);
            }
            
            ColumnLayout {
                anchors.fill:parent
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: false
                    text: {
                        if (teacherAction=="add") {
                            return i18nd("lliurex-homework-harvester","Adding the following folder as destination");
                        }
                        
                        if (teacherAction=="del") {
                            return i18nd("lliurex-homework-harvester","Removing the following folder as destination");
                        }
                    }
                }
                
                Kirigami.Icon {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.minimumWidth: Kirigami.Units.iconSizes.huge
                    Layout.minimumHeight: width
                    
                    source: "folder"
                    
                }
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: false
                    text: folderName
                }
                
                Kirigami.InlineMessage {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth:true
                    Layout.minimumHeight:32
                    
                    id: errorLabel
                    type: Kirigami.MessageType.Error
                }
                
                RowLayout {
                    Layout.alignment: Qt.AlignBottom | Qt.AlignRight
                    Layout.fillWidth: true
                    
                    QQC2.Button {
                        text: {
                            if (teacherAction=="add") {
                                return i18nd("lliurex-homework-harvester","Add")
                            }
                            
                            if (teacherAction=="del") {
                                return i18nd("lliurex-homework-harvester","Delete")
                            }
                            
                            return ""
                        }
                        
                        onClicked: {
                            errorLabel.visible = false;
                            
                            if (teacherAction=="add") {
                                enabled=false;
                                share_add_path.call(["",teacherTarget,folderName,"",""]);
                                register_share_info.call([userName,main.pswd,teacherTarget]);
                            }
                            
                            if (teacherAction=="del") {
                                enabled=false;
                                share_remove_path.call([""]);
                            }
                        }
                    }
                    
                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Close")
                        
                        onClicked: {
                            Qt.exit(0);
                        }
                    }
                }
            }
        }
    }
}
