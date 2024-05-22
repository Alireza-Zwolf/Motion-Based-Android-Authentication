import QtQuick 2.15
import QtQuick.Controls 2.15
import Sensors 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Sensor Data Capture")

    Column {
        anchors.centerIn: parent

        Button {
            text: "Start"
            onClicked: {
                accelerometerSensor.startCapturing()
                gyroscopeSensor.startCapturing()
            }
        }

        Button {
            text: "End"
            onClicked: {
                accelerometerSensor.stopCapturing()
                gyroscopeSensor.stopCapturing()
                accelerometerSensor.saveDataToJson("motion_rotation_data.json")
            }
        }
    }
}
