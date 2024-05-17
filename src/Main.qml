import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Material

import MotionBasedAuthentication 1.0

ApplicationWindow {
    width: 300
    height: 600

    // For maximization
    // visibility: "Maximized"

    visible: true
    title: qsTr("Motion-based Authentication")

    MotionBasedAuthentication {
        id: motionBasedAuthentication

        onResultChanged: function(result) {
            dialog.show(result)
        }

        onDataChanged: function(data) {
            textArea.text = data
        }
    }

    Label {
        width: 200
        height: 50
        x: 50
        y: 60

        text: "Result:"
        horizontalAlignment: Qt.AlignCenter
    }

    Flickable {
        width: 200
        height: 200
        x: 50
        y: 110

        flickableDirection: Flickable.VerticalFlick

        TextArea.flickable: TextArea {
            id: textArea
            anchors.fill: parent

            text: "No data"

            readOnly: true
            wrapMode: TextArea.Wrap
        }

        // To have scroll bar
        // ScrollBar.vertical: ScrollBar {}
    }

    Button {
        id: startRecordingBtn

        width: 200
        height: 50
        x: 50
        y: 320

        text: "Start Recording"

        onClicked: {
            motionBasedAuthentication.startRecording()
        }
    }

    Button {
        id: endRecordingBtn

        width: 200
        height: 50
        x: 50
        y: 380

        text: "End Recording"

        onClicked: {
            motionBasedAuthentication.endRecording()
        }
    }

    Button {
        id: authenticateBtn

        width: 200
        height: 50
        x: 50
        y: 440

        text: "Authenticate"

        onClicked: {
            motionBasedAuthentication.authenticate()
        }
    }

    Button {
        id: showResultBtn

        width: 200
        height: 50
        x: 50
        y: 500

        text: "Show Data"

        onClicked: {
            motionBasedAuthentication.showData()
        }
    }

    Dialog {
        id: dialog
        anchors.centerIn: parent
        width: 150
        height: 150
        x: 125
        y: 225

        Label {
            id: dialogMessage

            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
        }

        function show(result) {
            if (result === true) {
                dialogMessage.text = "Successfull"
                // To do: change color to green
            } else {
                dialogMessage.text = "Failure"
                // To do: change color to red
            }

            open()
        }

        // To have OK button
        // standardButtons: Dialog.Ok
    }
}
