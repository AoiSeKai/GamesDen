import QtQuick
import QtQuick.Controls

Window {
    id: debugWindow
    title: "🛠️ Sandbox & Dev Tools"
    width: 320
    height: 600
    visible: true
    color: "#16161a"

    property var gameEngine
    property var spritesLayer
    property string assetPath
    property var animalsList: []

    signal requestLevelUpSimulation(string animal1, string animal2)
    signal requestUpgradeSimulation(string animal1, string animal2)

    function refreshAnimals() {
        if (debugWindow.gameEngine) {
            debugWindow.animalsList = debugWindow.gameEngine.getAllPossibleAnimals()
            animal1Chooser.model = debugWindow.animalsList
            animal2Chooser.model = debugWindow.animalsList

            upgradedAnimal1.model = debugWindow.animalsList
            upgradedAnimal2.model = debugWindow.animalsList
            console.log("✅ Sandbox : gameEngine connecté ! Animaux détectés :", debugWindow.animalsList.length)
        } else {
            console.log("❌ Sandbox : gameEngine est toujours nul lors du refresh.")
        }
    }

    Component.onCompleted: {
        Qt.callLater(refreshAnimals);
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Button {
            text: "🔄 Refresh datas"
            width: parent.width
            onClicked: debugWindow.refreshAnimals()
        }

        Text {
            text: "🛠 DEBUG MODE"
            font.bold: true
            font.pointSize: 12
            color: "#fffffe"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // SIMULATE LEVEL UP : Will send a signal to Animals.qml to trigger the levelupView
        Rectangle {
            width: parent.width
            height: 180
            color: "#242629"
            radius: 6
            border.color: "#7f5af0"
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text {
                    text: "Configure Level Up"
                    color: "#94a1b2"
                    font.bold: true
                }

                // First animal
                Row {
                    spacing: 10
                    width: parent.width
                    Text { text: "Animal 1 :"; color: "#fffffe"; anchors.verticalCenter: parent.verticalCenter; width: 60 }
                    ComboBox {
                        id: animal1Chooser
                        width: parent.width - 70
                    }
                }

                // Second animal
                Row {
                    spacing: 10
                    width: parent.width
                    Text { text: "Animal 2 :"; color: "#fffffe"; anchors.verticalCenter: parent.verticalCenter; width: 60 }
                    ComboBox {
                        id: animal2Chooser
                        width: parent.width - 70
                    }
                }

                // Validate
                Rectangle {
                    id: simulateButton
                    width: parent.width
                    height: 40
                    radius: 4

                    color: mouseAreaBtn.pressed ? "#5b3a9c" : (mouseAreaBtn.containsMouse ? "#9370db" : "#7f5af0")

                    Text {
                        text: "Simulate level up"
                        color: "white"
                        font.bold: true
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        id: mouseAreaBtn
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            debugWindow.requestLevelUpSimulation(animal1Chooser.currentText, animal2Chooser.currentText)
                        }
                    }
                }
            }
        }

        // SIMULATE Upgrade : Will send a signal to Animals.qml to trigger the UpgradeView
        Rectangle {
            width: parent.width
            height: 180
            color: "#242629"
            radius: 6
            border.color: "#7f5af0"
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text {
                    text: "Configure Upgrade"
                    color: "#94a1b2"
                    font.bold: true
                }

                // First animal
                Row {
                    spacing: 10
                    width: parent.width
                    Text { text: "Animal 1 :"; color: "#fffffe"; anchors.verticalCenter: parent.verticalCenter; width: 60 }
                    ComboBox {
                        id: upgradedAnimal1
                        width: parent.width - 70
                    }
                }

                // Second animal
                Row {
                    spacing: 10
                    width: parent.width
                    Text { text: "Animal 2 :"; color: "#fffffe"; anchors.verticalCenter: parent.verticalCenter; width: 60 }
                    ComboBox {
                        id: upgradedAnimal2
                        width: parent.width - 70
                    }
                }

                // Validate
                Rectangle {
                    width: parent.width
                    height: 40
                    radius: 4

                    color: mouseAreaBtnUpgrade.pressed ? "#5b3a9c" : (mouseAreaBtnUpgrade.containsMouse ? "#9370db" : "#7f5af0")

                    Text {
                        text: "Simulate Upgrade"
                        color: "white"
                        font.bold: true
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        id: mouseAreaBtnUpgrade
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            debugWindow.requestUpgradeSimulation(upgradedAnimal1.currentText, upgradedAnimal2.currentText)
                        }
                    }
                }
            }
        }
    }
}