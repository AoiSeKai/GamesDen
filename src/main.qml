import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

/// @brief Main menu contains a list of games
Window {
    // default size (9:16) on mobile
    width: 360
    height: 640
    visible: true
    title: "GamesDen"
    color: "#1e1e24"

    // Stackview that allows navigating from menu to games
    StackView {
        id: mainStack
        anchors.fill: parent

        // Start with main menu by default
        initialItem: mainMenuComponent
    }

    Component {
        id: mainMenuComponent

        Item {
            id: menuRoot

            // List of games
            ListModel {
                id: gameModel
                ListElement {
                    title: "Animals";
                    placeholderColor: "#ff5e5b";
                    qrcPath: "qrc:/GamesDenApp/src/animals/Animals.qml" // Will load this qml when selected
                }
                ListElement { title: "TBD"; placeholderColor: "#00cecb" }
                ListElement { title: "TBD"; placeholderColor: "#ffed66" }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                // Title
                Text {
                    text: "SELECT A GAME"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                // drop-down list
                ListView {
                    id: gameList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: gameModel
                    clip: true // To prevent text from spilling when scrolling.
                    spacing: 15

                    // An element of the list
                    delegate: Rectangle {
                        width: gameList.width
                        height: 120
                        color: placeholderColor
                        radius: 12

                        // Visual effect (white border) if the element is selected
                        border.color: "white"
                        border.width: gameList.currentIndex === index ? 4 : 0

                        Text {
                            anchors.centerIn: parent
                            text: title
                            font.pixelSize: 24
                            font.bold: true
                            color: "#2b2b2b"
                        }

                        // Selectable zone
                        MouseArea {
                            anchors.fill: parent
                            onClicked: gameList.currentIndex = index
                        }
                    }
                }

                // Button to validate the selection
                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    color: playMouseArea.pressed ? "#3e8e41" : "#4CAF50" // Change color on click
                    radius: 12

                    Text {
                        anchors.centerIn: parent
                        // Get the name of the selected game
                        text: "PLAY " + gameModel.get(gameList.currentIndex).title.toUpperCase()
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    MouseArea {
                        id: playMouseArea
                        anchors.fill: parent
                        onClicked: {
                            var selectedGame = gameModel.get(gameList.currentIndex);

                            // Checks that qml is valid
                            if (selectedGame.qrcPath !== "") {
                                console.log("START : " + selectedGame.title);

                                // Push the game window
                                mainStack.push(selectedGame.qrcPath);
                            } else {
                                console.log("Game is not implemented yet.");
                            }
                        }
                    }
                }
            }
        }
    }
}