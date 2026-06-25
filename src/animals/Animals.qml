import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import GamesDenApp // To get access to C++ classes

Item { // Adds a wrapper that will go from Portrait or Landscape view
    id: wrapper
    // In release mode, the wrapper takes the whole screen.
    anchors.fill: IS_DEBUG_MODE ? undefined : parent

    readonly property string assetPath: "qrc:/animals/assets/"

    AnimalsGame {
        id: gameEngine

        onMatchOccurred: (indices) => {
            spritesLayer.playMatchEffect(indices)
        }
    }

    Connections {
        target: gameEngine
        function onRequestMovementAnimation(fromIndex, animalType, path) {
            spritesLayer.playMovement(fromIndex, animalType, path)
        }
    }

    // Called when we click on an empty cell
    // Will start the movement animation
    function startMovement(clickedIndex) {
        gameEngine.handleCellClick(clickedIndex)
    }


    Rectangle {
        id: gameRoot
        color: "#121212"

        anchors.centerIn: parent

        // In debug mode (on computer), we will fake a mobile landscape view
        width: IS_DEBUG_MODE ? wrapper.height : parent.width
        height: IS_DEBUG_MODE ? wrapper.width : parent.height
        rotation: IS_DEBUG_MODE ? 90 : 0

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // ==========================================
            // 1. Left part : HUD (Score, Infos)
            // ==========================================
            Rectangle {
                Layout.preferredWidth: parent.width * 0.25 // 25% of the screen
                Layout.fillHeight: true
                color: "#1e1e1e"

                // Border to separate on the right
                Rectangle {
                    width: 2
                    height: parent.height
                    anchors.right: parent.right
                    color: "#333333"
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 20

                    // Score
                    Column {
                        Layout.fillWidth: true
                        Text { text: "SCORE"; color: "#888888"; font.pixelSize: 12; font.bold: true }
                        Text { text: gameEngine.score.toString().padStart(6, '0'); color: "#deff9a"; font.pixelSize: 28; font.bold: true }
                    }

                    // Timer
                    Column {
                        Layout.fillWidth: true
                        Text { text: "LEVEL"; color: "#888888"; font.pixelSize: 12; font.bold: true }
                        Text { text: gameEngine.currentLevel.toString(); color: "white"; font.pixelSize: 24 }
                    }

                    // Queue of animals that will be placed on the grid
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 10
                        Text { text: "NEXTS"; color: "#888888"; font.pixelSize: 12; font.bold: true }

                        Repeater {
                            model: gameEngine.animalQueue // Get queue from C++
                            delegate: Rectangle {
                                width: 30; height: 30; radius: 8; color: index === 0 ? "#2a2a2a" : "#1a1a1a"
                                Layout.alignment: Qt.AlignHCenter
                                border.color: index === 0 ? "#deff9a" : "transparent"; border.width: 2
                                opacity: index === 0 ? 1.0 : 0.6

                                Image {
                                    anchors.fill: parent;
                                    anchors.margins: 2
                                    source: assetPath + modelData + ".png" // "modelData" is the name of the current element in a QStringList
                                    fillMode: Image.PreserveAspectFit
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true } // Space to push buttons at the bottom

                    // Menu button to go back to main menu.
                    Button {
                        text: "EXIT"
                        Layout.fillWidth: true
                        // @TODO: Back to main menu. Add dialog box to confirm and save.
                        onClicked: console.log("Retour au menu")
                    }
                }
            }

            // ==========================================
            // 2. Right part : Game grid
            // ==========================================
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Just a grid for the background
                GridLayout {
                    id: gameplayGrid
                    anchors.centerIn: parent

                    // Adjusting grid size and center it properly
                    width: Math.min(parent.width * 0.9, parent.height * 1.4)
                    height: width * (5/7)

                    columns: 7
                    rows: 5
                    rowSpacing: 5
                    columnSpacing: 5


                    Repeater {
                        model: gameEngine.grid
                        delegate: Rectangle {
                            id: cell

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            color: gameEngine.selectedIndex === index ? "#3d5afe" : "#2a2a2a"
                            radius: 4
                            border.color: gameEngine.selectedIndex === index ? "#deff9a" : "#333"
                            border.width: gameEngine.selectedIndex === index ? 2 : 1

                            MouseArea {
                                anchors.fill: parent
                                onClicked: startMovement(index) // Move animation
                            }
                        }
                    }
                }

                // Images that will move on top of the Grid
                AnimalSpritesLayer {
                    id: spritesLayer
                    anchors.fill: gameplayGrid

                    // Linking
                    gridModel: gameEngine.grid
                    assetPath: wrapper.assetPath
                    gridWidth: gameplayGrid.width
                    gridHeight: gameplayGrid.height
                    columns: gameplayGrid.columns
                    rows: gameplayGrid.rows
                    columnSpacing: gameplayGrid.columnSpacing
                    rowSpacing: gameplayGrid.rowSpacing

                    onMovementFinished: (fromIndex, toIndex) => {
                        gameEngine.finalizeMovement(fromIndex, toIndex)
                    }
                }
            }
        }

        // Overlay when leveling up
        LevelUpView {
            id: levelUpLayer
            anchors.fill: parent


            // Debug property
            property bool forceVisible: false
            property var debugOptions: []

            // Linking datas
            options: forceVisible ? debugOptions : gameEngine.upgradeOptions
            assetPath: wrapper.assetPath
            animalGameEngine: gameEngine

            // Display only on level up
            visible: gameEngine.isLevelingUp || forceVisible

            onAnimalSelected: (index) => {
                if (forceVisible) {
                    forceVisible = false
                } else {
                    gameEngine.selectNewAnimal(index)
                }
            }
        }

        // Overlay when upgrading
        UpgradeView {
            id: upgradeLayer
            anchors.fill: parent


            // Debug property
            property bool forceVisible: false
            property var debugOptions: []

            // Linking datas
            options: forceVisible ? debugOptions : gameEngine.upgradeOptions
            assetPath: wrapper.assetPath
            animalGameEngine: gameEngine

            // Display only on level up
            visible: gameEngine.isUpgrading || forceVisible

            onAnimalSelected: (index) => {
                if (forceVisible) {
                    forceVisible = false
                } else {
                    gameEngine.selectUpgrade(index)
                }
            }
        }
    }




    // DEBUG MODE ONLY
    Loader {
        id: debugLoader
        active: true // True = debug. Set to false to remove debug mode.

        sourceComponent: Component {
            DebugManager {
                // On laisse vide ici pour l'instant, on va injecter par le bas
                assetPath: wrapper.assetPath
            }
        }
        onStatusChanged: {
            if (debugLoader.status === Loader.Ready) {
                debugLoader.item.gameEngine = gameEngine
                debugLoader.item.spritesLayer = spritesLayer
                debugLoader.item.refreshAnimals()
            }
        }

        Connections {
            target: debugLoader.item

            function onRequestLevelUpSimulation(animal1, animal2) {
                console.log("Simulation du Level Up avec :", animal1, "et", animal2)
                levelUpLayer.debugOptions = [ animal1, animal2 ]
                levelUpLayer.forceVisible = true
            }

            function onRequestUpgradeSimulation(animal1, animal2) {
                console.log("Simulation de l'amélioration avec :", animal1, "et", animal2)
                upgradeLayer.debugOptions = [ animal1, animal2 ]
                upgradeLayer.forceVisible = true
            }


        }
    }
}