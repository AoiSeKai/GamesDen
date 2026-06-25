import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: root
    anchors.fill: parent
    color: "#F2121212"

    // WARNING: Properties that must be filled by parent
    property var options: []
    property string assetPath: ""
    property var animalGameEngine: null

    // Signal parent which choice was selected
    signal animalSelected(int index)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 15

        Text {
            text: "LEVEL UP"
            color: "#deff9a"
            font.pixelSize: 30
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "CHOOSE YOUR UPGRADE"
            color: "white"
            font.pixelSize: 17
            font.letterSpacing: 2
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            Repeater {
                model: root.options
                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.maximumHeight: 400
                    color: "#1e1e1e"
                    radius: 15
                    border.color: mouseArea.containsMouse ? "#deff9a" : "#333"
                    border.width: 2

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 10

                        Image {
                            source: root.assetPath + modelData + ".png"
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 70
                            Layout.alignment: Qt.AlignHCenter
                            fillMode: Image.PreserveAspectFit
                        }

                        // Name + Score
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 5
                            Layout.rightMargin: 5
                            Layout.alignment: Qt.AlignHCenter

                            Text {
                                text: modelData.toUpperCase()
                                color: "white"
                                font.pixelSize: 18
                                font.bold: true
                                Layout.alignment: Qt.AlignVCenter
                            }

                            Rectangle {
                                height: 24
                                width: scoreLayout.implicitWidth + 16
                                color: "#2a2a2a"
                                radius: 12
                                border.color: "#7f5af0"
                                border.width: 1
                                Layout.alignment: Qt.AlignVCenter

                                RowLayout {
                                    id: scoreLayout
                                    anchors.centerIn: parent
                                    spacing: 5

                                    Text {
                                        text: "⭐"
                                        font.pixelSize: 10
                                        Layout.alignment: Qt.AlignVCenter
                                    }

                                    // Previous score
                                    Text {
                                        text: root.animalGameEngine ? root.animalGameEngine.getAnimalScore(modelData) : "0"
                                        color: "#aaaaaa"
                                        font.pixelSize: 11
                                        font.bold: true
                                        font.strikeout: true
                                        Layout.alignment: Qt.AlignVCenter
                                    }


                                    Text {
                                        text: "➔"
                                        color: "#7f5af0"
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignVCenter
                                    }

                                    // New score
                                    Text {
                                        id: scoreText
                                        text: root.animalGameEngine ? root.animalGameEngine.getAnimalUpgradedScore(modelData) + " PTS" : "0 PTS"
                                        color: "#2cb67d"
                                        font.pixelSize: 11
                                        font.bold: true
                                        Layout.alignment: Qt.AlignVCenter
                                    }
                                }
                            }
                        }


                        // Invisible rectangle to force text to take place
                        Text {
                            text: root.animalGameEngine ? root.animalGameEngine.getAnimalPower(modelData) : "Error: Engine not found"
                            color: "#aaa"
                            font.pixelSize: 15
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.animalSelected(index)
                    }
                }
            }
        }
    }
}