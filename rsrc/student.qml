import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.16 as Kirigami

import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0

QQC2.Pane {
    
    visible:true
    width: 400
    height: 600
    anchors.centerIn:parent
    
    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            Qt.quit()
        }
        onRejected: {
            console.log("Canceled")
            Qt.quit()
        }
        
    }
    
    ListModel {
        id: alpha
            ListElement {
                name: "file1.png"
                
            }
            ListElement {
                name: "file2.png"
                
            }
    }
    
    ListModel {
        id: bravo
            ListElement {
                name: "teacher01"
                
            }
            ListElement {
                name: "teacher03"
                
            }
    }
    
    ColumnLayout {
        anchors.fill:parent
        
        ListView {
            //Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 250
            Layout.preferredHeight: 300
            //Layout.fillHeight: true
            
            model:alpha
            highlightFollowsCurrentItem: true
            
            delegate: Kirigami.BasicListItem {
                label: modelData
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
            }
        }
        
        ListView {
            //Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 250
            Layout.preferredHeight: 100
            
            model:bravo
            highlightFollowsCurrentItem: true
            
            delegate: Kirigami.BasicListItem {
                label: modelData
            }
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            
            QQC2.Button {
                text:"Cancel"
            }
            QQC2.Button {
                text:"Send"
                
            }
        }
        
    }
}
