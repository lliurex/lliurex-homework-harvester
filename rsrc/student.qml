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
    
    Component.onCompleted: {
        // HACK
        modelTeachers.append({"name":"teacher00"});
        
        for (var n=0;n<files.length;n++) {
            insertFile(files[n]);
        }
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
            Qt.quit()
        }
        
    }
    
    ListModel {
        id: modelFiles
    }
    
    ListModel {
        id: modelTeachers
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
        
        Kirigami.InlineMessage {
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth:true
            Layout.minimumHeight:32
            
            id: errorLabel
            type: Kirigami.MessageType.Error
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            
            QQC2.Button {
                text:i18nd("lliurex-homework-harvester","Send")
                
                onClicked: {
                    errorLabel.text="No N4D found";
                    errorLabel.visible=true;
                }
                
            }
            
            QQC2.Button {
                text:i18nd("lliurex-homework-harvester","Cancel")
                
                onClicked: {
                    Qt.Quit();
                }
            }
        }
        
    }
}
