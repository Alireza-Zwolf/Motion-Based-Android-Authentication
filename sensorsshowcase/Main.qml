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
                width: 300
                height: 300
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    // Draw box axes
                    ctx.strokeStyle = "black";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo(30, 30); // Start point
                    ctx.lineTo(width - 30, 30); // Top line
                    ctx.lineTo(width - 30, height - 30); // Right line
                    ctx.lineTo(30, height - 30); // Bottom line
                    ctx.lineTo(30, 30); // Left line
                    ctx.stroke();

                    // Function to draw arrowheads
                    function drawArrowhead(ctx, fromX, fromY, toX, toY) {
                        var headlen = 10; // length of head in pixels
                        var angle = Math.atan2(toY - fromY, toX - fromX);
                        ctx.beginPath();
                        ctx.moveTo(toX, toY);
                        ctx.lineTo(toX - headlen * Math.cos(angle - Math.PI / 6), toY - headlen * Math.sin(angle - Math.PI / 6));
                        ctx.moveTo(toX, toY);
                        ctx.lineTo(toX - headlen * Math.cos(angle + Math.PI / 6), toY - headlen * Math.sin(angle + Math.PI / 6));
                        ctx.stroke();
                    }

                    // Draw path data
                    ctx.strokeStyle = "blue";
                    ctx.lineWidth = 5;
                    ctx.beginPath();

                    if (pathData.length > 0) {
                        ctx.moveTo(40 + pathData[0].start.x * 1000, height - 40 - pathData[0].start.y * 1000);
                        for (var i = 0; i < pathData.length; i++) {
                            var segment = pathData[i];
                            if(segment.angle === 0){
                                var startX = 40 + segment.start.x * 1000;
                                var startY = height - 40 - segment.start.y * 1000;
                                var endX = 40 + segment.end.x * 1000;
                                var endY = height - 40 - segment.end.y * 1000;
                                ctx.lineTo(endX, endY);
                                ctx.stroke();
                                drawArrowhead(ctx, startX, startY, endX, endY);
                                ctx.beginPath();
                                ctx.moveTo(endX, endY);
                            }
                        }
                    }
                }
            }
        }

        Timer {
            id: resetColorTimer
            interval: 3000
            repeat: false
            onTriggered: background.color = "white"
        }
    }

    Connections {
        target: validator
        onValidationResult: {
            validationResult.text = result ? "Path is correct" : "Path is incorrect"
            background.color = result ? "green" : "red"
            resetColorTimer.start()
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
