// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtSensors

Item {
    id: root

    required property int fontSize
    required property int imageSize

    //! [0]
    Accelerometer {
        id: accelerometer

        // Variables to Store the current accelration values
        property real x: 0
        property real y: 0
        property real z: 0

        // Variables to store previous acceleration values
        property real prevX: 0
        property real prevY: 0
        property real prevZ: 0

        // Variables to store velocities
        property real vx: 0
        property real vy: 0
        property real vz: 0

        // Variables to store previous velocities
        property real prevVx: 0
        property real prevVy: 0
        property real prevVz: 0

        // Variables to store positions
        property real posX: 0
        property real posY: 0
        property real posZ: 0

        active: true
        dataRate: 100

        property real epochTime: 0.01

        onReadingChanged: {
            x = (reading as AccelerometerReading).x
            y = (reading as AccelerometerReading).y
            z = (reading as AccelerometerReading).z

            // Calculate change in velocity (delta v)
            var dvx = (x) * epochTime
            var dvy = (y) * epochTime
            var dvz = (z) * epochTime

            // Update velocity
            vx += dvx
            vy += dvy
            vz += dvz

            // Calculate change in position (delta s) using the average velocity
            // 1/2 at^2 + vt
            var dx = (0.5 * x * epochTime*epochTime) + (vx) * epochTime
            var dy = (0.5 * y * epochTime*epochTime) + (vy) * epochTime
            var dz = (0.5 * z * epochTime*epochTime) + (vz) * epochTime

            // Update position
            posX += dx
            posY += dy
            posZ += dz

            // Update previous values for next iteration
            prevX = x
            prevY = y
            prevZ = z
            prevVx = vx
            prevVy = vy
            prevVz = vz

            // Output the calculated position
            console.log("Acceleration(x, y, z):", x, y, z)
            // console.log("Position (x, y, z):", posX, posY, posZ)
        }
    }
    //! [0]
    // ColumnLayout {
    //     id: layout

    //     anchors.fill: parent
    //     spacing: 10

    //     Image {
    //         id: image

    //         Layout.alignment: Qt.AlignCenter
    //         Layout.preferredHeight: root.imageSize
    //         Layout.preferredWidth: root.imageSize
    //         fillMode: Image.PreserveAspectFit
    //         source: "images/qt_logo.png"

    //         transform: [
    //             Translate {
    //                 id: imageTranslation

    //                 x: 0
    //                 y: 0
    //             }
    //         ]
    //     }

    //     ProgressXYZBar {
    //         Layout.fillWidth: true
    //         fontSize: root.fontSize
    //         xText: "X: " + accelerometer.x.toFixed(2)
    //         xValue: 0.5 + (accelerometer.x / 100)
    //         yText: "Y: " + accelerometer.y.toFixed(2)
    //         yValue: 0.5 + (accelerometer.y / 100)
    //         zText: "Z: " + accelerometer.z.toFixed(2)
    //         zValue: 0.5 + (accelerometer.z / 100)
    //     }
    // }
}
