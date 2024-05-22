#include "accelerometersensor.h"
#include "qstandardpaths.h"
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>



const qreal DEFAULT_THRESHOLD = 0.4;  // Default threshold
const int DEFAULT_WINDOW_SIZE = 3;    // Default window size for moving average
const int ZERO_ACCEL_COUNT_LIMIT = 2; // Number of zero accelerations before velocity is reset
const int ZERO_ACCEL_COUNT_LIMIT_FOR_SAVING_DATA = 5; // Number of zero accelerations before velocity is reset
const qreal POSITION_THRESHOLD = 0.1;

AccelerometerSensor::AccelerometerSensor(QObject *parent)
    : QObject(parent),
    m_sensor(new QAccelerometer(this)),
    m_reading(nullptr),
    isCalibrated(false),
    initialX(0.0),
    initialY(0.0),
    initialZ(0.0),
    threshold(DEFAULT_THRESHOLD),
    windowSize(DEFAULT_WINDOW_SIZE),
    xSum(0.0),
    ySum(0.0),
    zSum(0.0),
    velocityX(0.0),
    velocityY(0.0),
    velocityZ(0.0),
    positionX(0.0),
    positionY(0.0),
    positionZ(0.0),
    latestX(0.0),
    latestY(0.0),
    latestZ(0.0),
    zeroAccelCount(0),
    zeroAccelCountTemp(0)
{
    connect(m_sensor, &QAccelerometer::readingChanged, this, &AccelerometerSensor::updateReading);
    lastUpdateTime.start();
    m_sensor->start();
}

AccelerometerSensor::~AccelerometerSensor()
{
    m_sensor->stop();
}

void AccelerometerSensor::calibrate()
{
    if (m_reading) {
        initialX = m_reading->x();
        initialY = m_reading->y();
        initialZ = m_reading->z();
        isCalibrated = true;
        qDebug() << "Calibration done - Initial X:" << initialX << " Y:" << initialY << " Z:" << initialZ;
    } else {
        qDebug() << "Reading is null, cannot calibrate!";
    }
}

void AccelerometerSensor::setThreshold(qreal newThreshold)
{
    threshold = newThreshold;
}

void AccelerometerSensor::updateReading()
{
    m_reading = m_sensor->reading();
    if (m_reading && capturing) {
        if (!isCalibrated) {
            calibrate(); // Calibrate on the first reading
        }

        applyDenoising();

        qreal deltaTime = lastUpdateTime.elapsed() / 1000.0; // Convert ms to s
        lastUpdateTime.restart();

        qreal currentX = m_reading->x();
        qreal currentY = m_reading->y();
        qreal currentZ = m_reading->z();

        if (currentX == 0 && currentY == 0 && currentZ == 0) {
            // qDebug() << "Zero Acceleration Detected";
            zeroAccelCount++;
            zeroAccelCountTemp++;
        } else {
            zeroAccelCount = 0;
        }

        if (zeroAccelCount >= ZERO_ACCEL_COUNT_LIMIT) {
            // qDebug() << "Resetting Velocity after several measurements of zero acceleration";
            velocityX = 0;
            velocityY = 0;
            velocityZ = 0;
        }

        // Update positions using the kinematic equation
        positionX += velocityX * deltaTime + 0.5 * currentX * qPow(deltaTime, 2);
        positionY += velocityY * deltaTime + 0.5 * currentY * qPow(deltaTime, 2);
        positionZ += velocityZ * deltaTime + 0.5 * currentZ * qPow(deltaTime, 2);

        // Update velocities
        velocityX += currentX * deltaTime;
        velocityY += currentY * deltaTime;
        velocityZ += currentZ * deltaTime;

        // emit positionChanged(positionX, positionY, positionZ);

        // Logging only significant changes
        // if(positionX > POSITION_THRESHOLD || positionY > POSITION_THRESHOLD || positionZ > POSITION_THRESHOLD)
        //     qDebug() << "Cur X:" << positionX << "Cur Y:" << positionY << "Cur Z:" << positionZ;

        // Logging the accelerations
        // if (fabs(m_reading->x()) > DEFAULT_THRESHOLD){
        //     qDebug() << "SignificantX: " << m_reading->x();
        // }
        // if (fabs(m_reading->y()) > DEFAULT_THRESHOLD){
        //     qDebug() << "SignificantY: " << m_reading->y();
        // }
        // if (fabs(m_reading->z()) > DEFAULT_THRESHOLD){
        //     qDebug() << "SignificantZ: " << m_reading->z();
        // }


        // Save the data to JSON
        if (zeroAccelCountTemp > ZERO_ACCEL_COUNT_LIMIT_FOR_SAVING_DATA){
            QJsonObject segment;
            segment["start"] = QJsonObject({{"x", latestX}, {"y", latestY}});
            segment["end"] = QJsonObject({{"x", positionX}, {"y", positionY}});
            qreal x_movement = fabs(positionX) - fabs(latestX);
            qreal y_movement = fabs(positionY) - fabs(latestY);

            // setting directions
            if (y_movement >= x_movement){
                if(positionY >= latestY)
                    segment["direction"] = "top";
                else
                    segment["direction"] = "button";
            }
            else{
                if(positionX >= latestX)
                    segment["direction"] = "right";
                else
                    segment["direction"] = "left";
            }


            // Do not store any data if the device is motionless
            if(latestX != positionX && latestY != positionY){
                currentPathSegment = segment;
                pathArray.append(currentPathSegment);
                zeroAccelCountTemp = 0;

                // Updating the Latest Position
                latestX = positionX;
                latestY = positionY;

                qDebug() << "New movement added to the path.";
            }
        }
        // Emit positionChanged signal
        emit positionChanged(positionX, positionY, positionZ);

    }
}

void AccelerometerSensor::applyDenoising()
{
    qreal rawX = m_reading->x() - initialX;
    qreal rawY = m_reading->y() - initialY;
    qreal rawZ = m_reading->z() - initialZ;

    qreal currentX = fabs(rawX) > threshold ? rawX : 0;
    qreal currentY = fabs(rawY) > threshold ? rawY : 0;
    qreal currentZ = fabs(rawZ) > threshold ? rawZ : 0;

    xQueue.enqueue(currentX);
    yQueue.enqueue(currentY);
    zQueue.enqueue(currentZ);

    xSum += currentX;
    ySum += currentY;
    zSum += currentZ;

    if (xQueue.size() > windowSize) {
        xSum -= xQueue.dequeue();
    }
    if (yQueue.size() > windowSize) {
        ySum -= yQueue.dequeue();
    }
    if (zQueue.size() > windowSize) {
        zSum -= zQueue.dequeue();
    }

    m_reading->setX(xSum / xQueue.size());
    m_reading->setY(ySum / yQueue.size());
    m_reading->setZ(zSum / zQueue.size());
}

void AccelerometerSensor::startCapturing() {
    capturing = true;
    pathArray = QJsonArray();  // Initialize the JSON array
    currentPathSegment = QJsonObject(); // Initialize current path segment
    lastUpdateTime.start();
}

void AccelerometerSensor::stopCapturing() {
    capturing = false;
    // if (!currentPathSegment.isEmpty()) {
    //     pathArray.append(currentPathSegment);
    // }
}

void AccelerometerSensor::addRotationData(int degrees) {
    if (capturing) {
        currentPathSegment["start"] = QJsonObject({{"x", latestX}, {"y", latestY}});
        currentPathSegment["end"] = QJsonObject({{"x", latestX}, {"y", latestY}});
        currentPathSegment["direction"] = degrees > 0 ? "right" : "left";
        currentPathSegment["angle"] = degrees;
        pathArray.append(currentPathSegment);
        qDebug() << "New rotation added to the path.";
    }
}

void AccelerometerSensor::saveDataToJson(const QString &filename) {
    QJsonObject rootObj;
    rootObj["path"] = pathArray;
    QJsonDocument doc(rootObj);

    // QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + filename;

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString filePath = documentsPath + "/" + filename;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "File saved to the json file.";
        qDebug() << pathArray;
    } else {
        qWarning("Couldn't open save file.");
    }
    qDebug() << "Current working directory:" << QDir::currentPath();

}
