import QtQuick 2.15
import QtQuick.Controls 2.15
import Sensors 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Sensor Data Capture")

    Rectangle {
        id: background
        anchors.fill: parent
        color: "white"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            component CustomButton: Button {
                                highlighted: true
                                font.pixelSize: root.defaultFontSize
                                font.letterSpacing: 1.5

                                Layout.alignment: Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }

            CustomButton {
                text: "Start"
                width: 200
                height: 50
                onClicked: {
                    accelerometerSensor.startCapturing()
                    gyroscopeSensor.startCapturing()
                }
            }

            CustomButton {
                text: "End"
                width: 200
                height: 50
                onClicked: {
                    accelerometerSensor.stopCapturing()
                    gyroscopeSensor.stopCapturing()
                    accelerometerSensor.saveDataToJson("motion_rotation_data.json")
                }
            }

            CustomButton {
                text: "Validate Path"
                width: 200
                height: 50
                onClicked: {
                    validator.validatePath()
                }
            }

            Text {
                id: validationResult
                text: ""
                font.pixelSize: 20
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Connections {
        target: validator
        onValidationResult: {
            validationResult.text = result ? "Path is correct" : "Path is incorrect"
            background.color = result ? "green" : "red"
            Qt.createQmlObject('import QtQuick 2.0; Timer { interval: 3000; repeat: false; onTriggered: background.color = "white" }', background, "colorResetTimer")
        }
    }
}
