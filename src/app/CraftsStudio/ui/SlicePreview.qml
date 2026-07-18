import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick3D 6.0
import QtQuick3D.Helpers 6.0

Rectangle {
    color: "#1a1a2e"

    View3D {
        id: view3D
        anchors.fill: parent
        anchors.bottomMargin: 60

        environment: SceneEnvironment {
            clearColor: "#1a1a2e"
            backgroundMode: SceneEnvironment.Color
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(100, 80, 150)
            eulerRotation: Qt.vector3d(-25, -30, 0)
        }

        DirectionalLight {
            eulerRotation: Qt.vector3d(-30, -45, 0)
        }

        // Build plate reference
        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(2, 2, 0.02)
            position: Qt.vector3d(100, 100, -0.5)
            materials: [ DefaultMaterial { diffuseColor: "#333355" } ]
        }

        // Current layer outline
        Node {
            id: layerNode
            position: Qt.vector3d(0, 0, 0)

            Repeater3D {
                model: layerLineModel
                // Lines rendered as thin quads connecting consecutive points
            }
        }

        // Axes indicator
        Node {
            // Simple origin marker
        }

        // OrbitCameraController provides mouse interaction
        OrbitCameraController {
            camera: camera
            origin: camera
        }
    }

    // Bottom controls
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: "#16213e"

        Row {
            anchors.centerIn: parent
            spacing: 15

            Button {
                text: "< 前层"
                onClicked: { if (bridge) bridge.prevLayer() }
            }

            Slider {
                id: layerSlider
                width: 300
                from: 0
                to: bridge ? bridge.totalLayers - 1 : 0
                value: bridge ? bridge.currentLayer : 0
                onValueChanged: { if (bridge) bridge.currentLayer = value }
            }

            Button {
                text: "后层 >"
                onClicked: { if (bridge) bridge.nextLayer() }
            }

            Text {
                text: bridge ? ("层 " + bridge.currentLayer + "/" + bridge.totalLayers +
                      "  Z=" + bridge.layerZ.toFixed(2) + "mm") : "尚未切片"
                color: "white"
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    // C++ bridge context property
    property var bridge: null
    property var layerLineModel: ListModel { }
}
