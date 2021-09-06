import Edupals.N4D 1.0 as N4D

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.16 as Kirigami

import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0

QQC2.Pane {
    
    visible:true
    width: 400
    height: 360
    anchors.centerIn: parent
    
    N4D.Client {
        id: n4d
        address: "https://192.168.122.147:9779"
        credential: N4D.Client.Anonymous
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
        }
    }
    
    N4D.Proxy {
        id: share_is_configured
        client: n4d
        plugin:"TeacherShareManager"
        method:"is_configured"
        
        onResponse: {
            console.log("is configured:",value);
        }
        
        onError: {
            console.log("n4d error:\n",what);
        }
    }
    
    QQC2.StackView {
        id: stack
        initialItem: loginView
        anchors.fill: parent
    }
    
    Component {
        id: loginView
        
        QQC2.Pane {
            
            GridLayout {
                rows:3
                columns:2
                
                QQC2.Label {
                    text:i18nd("n4d-qt-agent","User")
                    //anchors.verticalCenter: userField.verticalCenter
                    Layout.row:0
                    Layout.column:0
                    Layout.alignment: Qt.AlignRight
                }
                
                QQC2.TextField {
                    id: userField
                    text:""
                    focus: true
                    
                    Layout.row:0
                    Layout.column:1
                }
                
                QQC2.Label {
                    text:i18nd("n4d-qt-agent","Password")
                    Layout.row:1
                    Layout.column:0
                    Layout.alignment: Qt.AlignRight
                }
                
                QQC2.TextField {
                    id: passwordField
                    echoMode: TextInput.Password
                    Layout.row:1
                    Layout.column:1
                    
                    onAccepted: {
                        
                    }
                }
            }
        }
    }
    
    Component {
        id: mainView
        
        QQC2.Pane {
            property string folderName : teacherTarget.split('/').reverse()[0]
            
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
                        text: i18nd("lliurex-homework-harvester",teacherAction)
                        
                        onClicked: {
                            share_get_paths.call([]);
                            errorLabel.text="No N4D found";
                            errorLabel.visible=true;
                        }
                    }
                    
                    QQC2.Button {
                        text: i18nd("lliurex-homework-harvester","Cancel")
                        
                        onClicked: {
                            Qt.quit();
                        }
                    }
                }
            }
        }
    }
}
