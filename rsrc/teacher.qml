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
                id: n4dLocal
                address: "https://192.168.122.147:9779"
                credential: N4D.Client.Password
                user: userName
            }
            
            N4D.Proxy {
                id: validate_auth
                client: n4dLocal
                method:"validate_auth"
                
                onResponse: {
                    //console.log("status",value[0]);
                    //console.log("status",value[1]);
                    passwordField.enabled = true;
                    main.pswd = passwordField.text;
                    passwordField.text = "";
                    stack.push(mainView);
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLogin.text = "Login failed";
                    errorLogin.visible = true;
                    passwordField.enabled = true;
                    passwordField.text = "";
                }
            }
            
            ColumnLayout {
                anchors.fill:parent
                
                QQC2.Label {
                    
                    text:"homework harvester needs your teacher password to perform this action"
                    wrapMode:Text.WordWrap
                    //anchors.verticalCenter: userField.verticalCenter
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth:true
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
                    echoMode: TextInput.Password
                    Layout.alignment: Qt.AlignHCenter
                    
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
                            //n4dLocal.password = passwordField.text
                            validate_auth.call([[userName,passwordField.text]]);
                        }
                    }
                    
                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Cancel")
                        
                        onClicked: {
                            Qt.exit(-1);
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
            
            N4D.Client {
                id: n4d
                address: "https://192.168.122.147:9779"
                credential: N4D.Client.Password
            }
            
            N4D.Proxy {
                id: share_get_paths
                client: n4d
                plugin:"TeacherShareManager"
                method:"get_paths"
                
                onResponse: {
                    console.log("paths:");
                    for (var v=0;v<value.length;v++) {
                        console.log(v);
                    }
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=what;
                    errorLabel.visible=true;
                }
            }
            
            N4D.Proxy {
                id: share_is_configured
                client: n4d
                plugin:"TeacherShareManager"
                method:"is_configured"
                
                onResponse: {
                    console.log("is configured:",value);
                    
                    if (teacherAction=="del") {
                        if (value==false) {
                            errorLabel.text=i18nd("lliurex-homework-harvester","This share is not configured");
                            errorLabel.visible=true;
                        }
                        else {
                        }
                    }
                    
                    if (teacherAction=="add") {
                        if (value==true) {
                            errorLabel.text=i18nd("lliurex-homework-harvester","This share is already configured");
                            errorLabel.visible=true;
                        }
                        else {
                            share_add_path.call(["",teacherTarget,folderName,"",""]);
                        }
                    }
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=i18nd("lliurex-homework-harvester","Error checking share status");
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
                    
                    if (value) {
                        Qt.exit(0);
                    }
                }
                
                onError: {
                    console.log("n4d error:\n",what);
                    errorLabel.text=i18nd("lliurex-homework-harvester","Can not add share");
                    errorLabel.visible=true;
                }
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
                            n4d.user = userName;
                            n4d.password = main.pswd;
                            
                            share_is_configured.call(["",teacherTarget]);
                            
                        }
                    }
                    
                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Cancel")
                        
                        onClicked: {
                            Qt.exit(-1);
                        }
                    }
                }
            }
        }
    }
}
