import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("GamesDen - Prototype")
    color: "#2c3e50"

    Text {
        anchors.centerIn: parent
        text: "Test"
        color: "white"
        font.pixelSize: 24
        font.bold: true
    }
}