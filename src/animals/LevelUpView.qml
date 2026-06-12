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

    // Signal parent which choice was selected
    signal animalSelected(int index)

    // @TODO Placeholder - Will be moved to C++
    function getDescription(animal) {
        const powers = {
            "cow": "Produit du lait : Double les points des voisins.",
            "chicken": "Effet de groupe : +50 pts par poulet adjacent.",
            "dog": "Gardien : Empeche les predateurs d'approcher.",
            "rabbit": "Prolifique : Se multiplie sur une case vide.",
            "snake": "Chasseur : Mange les petits animaux alentours.",
            "panda": "Zen : Stabilise le score du niveau.",
            "gorilla": "Puissance : Ecrase les cases bloquees.",
            "monkey": "Agile : Peut se deplacer apres la pose.",
            "crocodile": "Aquatique : Bonus sur les bords de grille.",
            "whale": "Geant : Occupe 2 cases au lieu d'une.",
            "narwhal": "Perceur : Aligne les scores en colonne.",
            "hippo": "Affame : Consomme les bonus de la grille."
        };
        return powers[animal] || "Un mysterieux animal aux pouvoirs caches...";
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 15

        Text {
            text: "LEVEL UP"
            color: "#deff9a"
            font.pixelSize: 36
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "CHOOSE YOUR NEW ANIMAL"
            color: "white"
            font.pixelSize: 20
            font.letterSpacing: 2
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 30

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

                        Text {
                            text: modelData.toUpperCase()
                            color: "white"
                            font.pixelSize: 20
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }

                        // Invisible rectangle to force text to take place
                        Text {
                            text: root.getDescription(modelData)
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