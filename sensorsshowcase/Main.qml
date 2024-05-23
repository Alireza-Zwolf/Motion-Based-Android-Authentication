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
                font.pixelSize: 16
                font.letterSpacing: 1.5

                Layout.alignment: Qt.AlignCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: 20

                CustomButton {
                    text: "Start Calibration"
                    width: 200
                    height: 50
                    enabled: !accelerometerSensor.capturing && !gyroscopeSensor.capturing
                    onClicked: {
                        accelerometerSensor.startCalibration()
                        gyroscopeSensor.startCalibration()
                    }
                }

                CustomButton {
                    text: "Stop Calibration"
                    width: 200
                    height: 50
                    enabled: !accelerometerSensor.capturing && !gyroscopeSensor.capturing
                    onClicked: {
                        accelerometerSensor.stopCalibration()
                        gyroscopeSensor.stopCalibration()
                    }
                }
            }

            CustomButton {
                text: "Start Capturing"
                width: 200
                height: 50
                onClicked: {
                    accelerometerSensor.startCapturing()
                    gyroscopeSensor.startCapturing()
                }
            }

            CustomButton {
                text: "Stop Capturing"
                width: 200
                height: 50
                onClicked: {
                    accelerometerSensor.stopCapturing()
                    gyroscopeSensor.stopCapturing()
                    accelerometerSensor.saveDataToJson("motion_rotation_data.json")
                    processPathArray(accelerometerSensor.getPathArray())
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

            Canvas {
                id: graphCanvas
                width: 400
                height: 400
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    // Draw axes
                    ctx.strokeStyle = "black";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo(40, 0);
                    ctx.lineTo(40, height);
                    ctx.lineTo(width, height);
                    ctx.stroke();

                    // Draw path data
                    ctx.strokeStyle = "red";
                    ctx.lineWidth = 5;
                    ctx.beginPath();

                    if (pathData.length > 0) {
                        ctx.moveTo(40 + pathData[0].start.x * 100 , height - 40 - pathData[0].start.y * 100);
                        for (var i = 0; i < pathData.length; i++) {
                            var segment = pathData[i];
                            ctx.lineTo(40 + segment.end.x * 100 , height - 40 - segment.end.y * 100);
                        }
                    }
                    ctx.stroke();
                }
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

    function processPathArray(pathArray) {
        pathData = pathArray;
        graphCanvas.requestPaint();
    }

    property var pathData: []

    Component.onCompleted: {
        processPathArray(accelerometerSensor.getPathArray());
    }
}
