import Edupals.N4D.Agent 1.0 as N4DAgent

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.16 as Kirigami

import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0

QQC2.Pane {
    
    visible:true
    width: 400
    height: 400
    anchors.centerIn: parent
    
    QQC2.StackView {
        id: stack
        initialItem: loginView
        anchors.fill: parent
    }
    
    Component {
        id: loginView
        
        QQC2.Pane {
            
            N4DAgent.Login {
                id: loginWidget
                
                //showAddress: true
                //address: "https://localhost:9779"
                //user: "netadmin"
                showCancel: true
                //trustLocal: true
                message: "Introduce your teacher credentials"
                //inGroups: ["teachers"]
                
                anchors.centerIn: parent
                
                onLogged: {
                    stack.push(mainView);
                }
                
                onCanceled: {
                    Qt.quit();
                }
            }
        }
    }
    
    Component {
        id: mainView
        QQC2.Pane {
            ColumnLayout {
                anchors.fill:parent
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                    text: teacherAction+" "+teacherTarget
                }
                
                RowLayout {
                    Layout.alignment: Qt.AlignBottom | Qt.AlignRight
                    Layout.fillWidth: true
                    
                    QQC2.Button {
                        text: "Add"
                        
                        onClicked: {
                            stack.push(waitView);
                        }
                    }
                    
                    QQC2.Button {
                        text: "Cancel"
                    }
                }
            }
        }
    }
    
    Component {
        id: waitView
        QQC2.Pane {
            ColumnLayout {
                anchors.fill:parent
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                    text: "working..."
                }
            }
        }
    }
}
