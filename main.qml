import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0

import QtQuick.Dialogs 1.3

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Display-Manager")


    function urlListToString(fileUrls) {
        var str = ""
        for(var i in fileUrls){
            var url = fileUrls[i]
            str += Qt.resolvedUrl(url) + "\n"
        }
        return str
    }

    TabView {
        width: parent.width
        height: 20

        Tab {
            title: "Acquisition"
            anchors.topMargin: 9
            anchors.leftMargin: 9

            ColumnLayout {
                Settings {
                    category: "display_manager"
                    id: set
                    property string mask_file: ""
                    property string image_files: ""
                }
                RowLayout {
                    ColumnLayout {
                        FileDialog {
                            id: maskDialog
                            title: "Please choose a mask file"
                            folder: shortcuts.home
                            onAccepted: {
                                console.log("You chose: " + fileUrls)
                                set.mask_file = urlListToString(fileUrls)
                                displayManager.setMaskFile(set.mask_file)
                                mask_display.text = set.mask_file
                            }
                            onRejected: {
                                console.log("Canceled opening mask image")
                            }
                            Component.onCompleted: {
                                displayManager.setMaskFile(set.mask_file)
                                mask_display.text = set.mask_file
                            }
                        }
                        Button {
                            text: "Open mask image"
                            onClicked: maskDialog.visible = true
                        }
                        Text {
                            id: mask_display
                            text: set.mask_file
                        }
                    }
                }
                RowLayout {
                    ColumnLayout {
                        Button {
                            text: "Add images to the animation"
                            onClicked: imageDialog.visible = true
                        }
                        Button {
                            text: "Clear animation"
                            onClicked: set.image_files = ""
                        }
                        FileDialog {
                            id: imageDialog
                            title: "Please choose an image"
                            folder: shortcuts.home
                            onAccepted: {
                                console.log("You chose: " + fileUrls)
                                set.image_files += urlListToString(fileUrls)
                                displayManager.setImageFiles(set.image_files)
                                image_display.text = set.image_files
                            }
                            onRejected: {
                                console.log("Canceled adding an image to the animation")
                            }
                            Component.onCompleted: {
                                displayManager.setImageFiles(set.image_files)
                            }
                        }
                        Text {
                            id: image_display
                            text: set.image_files
                        }
                    }
                }
                RowLayout {
                    Button {
                        text: "Send to uC"
                        onClicked: displayManager.sendToUC();
                    }
                }
            }
        }
    }
}
