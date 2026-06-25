import QtQuick

/// @brief This class is used to handle a Sprite on the grid of Animals.qml
/// The sprite must be moved from a cell to another cell with an animation
Item {
    id: root
    
    // --- WARNING: Configured from parent ---
    property var gridModel        // C++ grid (gameEngine.grid)
    property string assetPath     // Path to sprites
    property real gridWidth
    property real gridHeight
    property int columns: 7
    property int rows: 5
    property real columnSpacing: 6
    property real rowSpacing: 6

    // --- Signal for parent ---
    signal movementFinished(int fromIndex, int toIndex)

    // --- For the animation ---
    property var currentPath: []
    property int startCellIndex: -1
    property int currentCellIndex: -1
    property string animatedAnimalType: ""
    property bool isAnimating: false

    /// @brief Move an animal following the given path
    function playMovement(startIdx, animalType, path) {
        root.isAnimating = false // Cut animation at the beginning to avoid a "jump" to cell 1
        root.startCellIndex = startIdx
        root.currentCellIndex = startIdx
        root.animatedAnimalType = animalType
        root.currentPath = path
        movementTimer.start()
    }

    /// @brief Play an effect when we get a match
    function playMatchEffect(indices) {
        // For every destroyed cells
        for (let i = 0; i < indices.length; i++) {
            let idx = indices[i];

            // Compute the explosion coordinates
            let cellWidth = (root.gridWidth - ((root.columns - 1) * root.columnSpacing)) / root.columns;
            let cellHeight = (root.gridHeight - ((root.rows - 1) * root.rowSpacing)) / root.rows;
            let effectX = (idx % root.columns) * (cellWidth + root.columnSpacing);
            let effectY = Math.floor(idx / root.columns) * (cellHeight + root.rowSpacing);

            // Creates a pop effect
            popEffectComponent.createObject(root, {
                "x": effectX,
                "y": effectY,
                "width": cellWidth,
                "height": cellHeight
            });
        }
    }

    /// @brief Pop effect when we get a match
    Component {
        id: popEffectComponent

        Rectangle {
            id: popRect
            color: "transparent"
            border.color: "#deff9a"
            border.width: 4
            radius: width / 2
            opacity: 1.0
            scale: 0.5 // Starting small and will grow

            // Animation will grow and fade away
            ParallelAnimation {
                running: true
                NumberAnimation { target: popRect; property: "scale"; to: 1.8; duration: 300; easing.type: Easing.OutCubic }
                NumberAnimation { target: popRect; property: "opacity"; to: 0.0; duration: 300; easing.type: Easing.OutCubic }

                onFinished: popRect.destroy()
            }
        }
    }

    // Timer to animate the movement
    Timer {
        id: movementTimer
        interval: 80
        repeat: true
        onTriggered: {
            root.isAnimating = true
            if (root.currentPath.length > 0) {
                root.currentCellIndex = root.currentPath.shift()
            } else {
                movementTimer.stop()
                
                // Temporary save
                let finalFrom = root.startCellIndex
                let finalTo = root.currentCellIndex
                
                // Reset
                root.startCellIndex = -1
                root.currentCellIndex = -1
                root.animatedAnimalType = ""
                root.isAnimating = false

                // Let C++ handle the rest
                root.movementFinished(finalFrom, finalTo)
            }
        }
    }

    // Fixed sprite = those who are not animated
    Repeater {
        model: root.gridModel
        delegate: Item {

            readonly property real cellWidth: (root.gridWidth - ((root.columns - 1) * root.columnSpacing)) / root.columns
            readonly property real cellHeight: (root.gridHeight - ((root.rows - 1) * root.rowSpacing)) / root.rows
            
            width: cellWidth
            height: cellHeight

            x: (index % root.columns) * (cellWidth + root.columnSpacing)
            y: Math.floor(index / root.columns) * (cellHeight + root.rowSpacing)

            readonly property bool isMovingThisOne: root.startCellIndex === index || (root.currentCellIndex === index && movementTimer.running)

            Image {
                width: parent.width - 6
                height: parent.height - 6
                anchors.centerIn: parent
                source: (modelData !== "" && !isMovingThisOne) ? root.assetPath + modelData + ".png" : ""
                fillMode: Image.PreserveAspectFit
                visible: modelData !== "" && !isMovingThisOne
            }
        }
    }

    // The animated sprite
    Image {
        id: activeMovingSprite
        readonly property real cellW: (root.gridWidth - ((root.columns - 1) * root.columnSpacing)) / root.columns
        readonly property real cellH: (root.gridHeight - ((root.rows - 1) * root.rowSpacing)) / root.rows

        width: cellW - 6
        height: cellH - 6
        fillMode: Image.PreserveAspectFit
        
        visible: root.animatedAnimalType !== ""
        source: visible ? root.assetPath + root.animatedAnimalType + ".png" : ""

        x: root.currentCellIndex >= 0 ? ((root.currentCellIndex % root.columns) * (cellW + root.columnSpacing)) + 3 : 0
        y: root.currentCellIndex >= 0 ? (Math.floor(root.currentCellIndex / root.columns) * (cellH + root.rowSpacing)) + 3 : 0

        Behavior on x { enabled: root.isAnimating; NumberAnimation { duration: 110; easing.type: Easing.OutQuad } }
        Behavior on y { enabled: root.isAnimating; NumberAnimation { duration: 110; easing.type: Easing.OutQuad } }

        // Small animation to make it look like rabbit is jumping.
        transform: Scale {
            xScale: root.isAnimating && root.animatedAnimalType === "rabbit" ? 1.2 : 1.0
            yScale: root.isAnimating && root.animatedAnimalType === "rabbit" ? 1.2 : 1.0
        }
    }
}