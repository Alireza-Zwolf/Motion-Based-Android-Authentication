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

        // Variables to Store the current accelration values, with respect to their bias
        property real x: 0
        property real y: 0
        property real z: 0

        // Bias of measured values
        property real x_bias: 0.11
        property real y_bias: 0.24
        property real z_bias: 9.78

        // Kalman filter parameters
        property real q_x: 0.001 // process noise covariance
        property real r_x: 0.01 // measurement noise covariance
        property real p_x: 1 // initial estimate error covariance
        property real k_x: 0 // Kalman gain

        property real q_y: 0.001 // process noise covariance
        property real r_y: 0.01 // measurement noise covariance
        property real p_y: 1 // initial estimate error covariance
        property real k_y: 0 // Kalman gain

        property real q_z: 0.001 // process noise covariance
        property real r_z: 0.01 // measurement noise covariance
        property real p_z: 1 // initial estimate error covariance
        property real k_z: 0 // Kalman gain

        // Variables to store previous acceleration values
        // property real prevX: 0
        // property real prevY: 0
        // property real prevZ: 0

        // State estimates
        property real x_est: 0
        property real y_est: 0
        property real z_est: 0

        // Variables to store velocities
        property real vx: 0
        property real vy: 0
        property real vz: 0

        // Variables to store previous velocities
        // property real prevVx: 0
        // property real prevVy: 0
        // property real prevVz: 0

        // Variables to store positions
        property real posX: 0
        property real posY: 0
        property real posZ: 0

        active: true
        dataRate: 100

        property real epochTime: 0.01
        property real counter: 0


        onReadingChanged: {
            // Kalman Filter for x
            x = (reading as AccelerometerReading).x
            x = Number(x.toFixed(2))                // Rounding floating point digits to 2
            p_x = p_x + q_x
            k_x = p_x / (p_x + r_x)
            x_est = x_est + k_x * (x - x_est)
            p_x = (1 - k_x) * p_x

            // Kalman Filter for y
            y = (reading as AccelerometerReading).y
            y = Number(y.toFixed(2))                // Rounding floating point digits to 2
            p_y = p_y + q_y
            k_y = p_y / (p_y + r_y)
            y_est = y_est + k_y * (y - y_est)
            p_y = (1 - k_y) * p_y

            // Kalman Filter for z
            z = (reading as AccelerometerReading).z
            z = Number(z.toFixed(2))                // Rounding floating point digits to 2
            z -= 9.8
            p_z = p_z + q_z
            k_z = p_z / (p_z + r_z)
            z_est = z_est + k_z * (z - z_est)
            p_z = (1 - k_z) * p_z

            // if (counter == 0){
            // Subtracring estimations from the measured accelerations
            // x_est -= x_bias
            // y_est -= y_bias
            console.log(z)
            // z -= 9.8
            // console.log(z)
            console.log(z_est)
            // z_est -= 9.8
            // console.log(z_est)
            // }


            // Calculate change in velocity (delta v)
            var dvx = (x_est) * epochTime
            var dvy = (y_est) * epochTime
            var dvz = (z_est) * epochTime

            // Update velocity
            vx += dvx
            vy += dvy
            vz += dvz

            // Calculate change in position (delta s) using the average velocity
            // 1/2 at^2 + vt
            var dx = (0.5 * x_est * epochTime*epochTime) + (vx) * epochTime
            var dy = (0.5 * y_est * epochTime*epochTime) + (vy) * epochTime
            var dz = (0.5 * z_est * epochTime*epochTime) + (vz) * epochTime

            // Update position
            posX += dx
            posY += dy
            posZ += dz

            // Update previous values for next iteration
            // prevX = x
            // prevY = y
            // prevZ = z
            // prevVx = vx
            // prevVy = vy
            // prevVz = vz

            // Output the calculated position
            // console.log("Acceleration(x, y, z):", x, y, z)
            // if (counter == 0){
                console.log("Position (x, y, z):", posX, posY, posZ)
                console.log("Acceleration(x, y, z):", x_est, y_est, z_est)
                console.log("\n")
            // }
            counter += 1
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
