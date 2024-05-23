#include "accelerometersensor.h"
#include "qstandardpaths.h"
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <algorithm>

const qreal DEFAULT_THRESHOLD = 0.3;  // Adjusted threshold for better sensitivity
const int DEFAULT_WINDOW_SIZE = 3;    // Adjusted window size for better smoothing
const int ZERO_ACCEL_COUNT_LIMIT = 2; // Increased limit for better accuracy in zero detection
const int ZERO_ACCEL_COUNT_LIMIT_FOR_SAVING_DATA = 3;
const qreal RELOCATION_THRESHOLD = 0.2; // Adjusted for better small movement detection

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
    zeroAccelCountTemp(0),
    isCalibrating(false)
{
    connect(m_sensor, &QAccelerometer::readingChanged, this, &AccelerometerSensor::updateReading);
    lastUpdateTime.start();
    m_sensor->start();
}

AccelerometerSensor::~AccelerometerSensor()
{
    m_sensor->stop();
}

void AccelerometerSensor::setThreshold(qreal newThreshold)
{
    threshold = newThreshold;
}

void AccelerometerSensor::updateReading()
{
    m_reading = m_sensor->reading();
    if (m_reading && capturing) {
        if (isCalibrating) {
            calibrationXValues.append(m_reading->x());
            calibrationYValues.append(m_reading->y());
            calibrationZValues.append(m_reading->z());
        }

        qint64 currentTime = lastUpdateTime.elapsed();
        qreal deltaTime = currentTime / 1000.0; // Convert ms to s
        lastUpdateTime.restart();

        applyDenoising();

        qreal currentX = m_reading->x();
        qreal currentY = m_reading->y();
        qreal currentZ = m_reading->z();

        if (qAbs(currentX) < threshold && qAbs(currentY) < threshold && qAbs(currentZ) < threshold) {
            zeroAccelCount++;
            zeroAccelCountTemp++;
        } else {
            zeroAccelCount = 0;
        }

        if (zeroAccelCount >= ZERO_ACCEL_COUNT_LIMIT) {
            velocityX = 0;
            velocityY = 0;
            velocityZ = 0;
        }

        if (zeroAccelCountTemp >= ZERO_ACCEL_COUNT_LIMIT_FOR_SAVING_DATA){
            positionX += velocityX * deltaTime + 0.5 * currentX * deltaTime * deltaTime;
            positionY += velocityY * deltaTime + 0.5 * currentY * deltaTime * deltaTime;
            positionZ += velocityZ * deltaTime + 0.5 * currentZ * deltaTime * deltaTime;

            velocityX += currentX * deltaTime;
            velocityY += currentY * deltaTime;
            velocityZ += currentZ * deltaTime;

            QJsonObject segment;
            segment["start"] = QJsonObject({{"x", latestX}, {"y", latestY}});
            segment["end"] = QJsonObject({{"x", positionX}, {"y", positionY}});
            qreal xMovement = qAbs(positionX - latestX);
            qreal yMovement = qAbs(positionY - latestY);

            if (yMovement >= xMovement){
                if(positionY >= latestY)
                    segment["direction"] = "up";
                else
                    segment["direction"] = "down";
            }
            else{
                if(positionX >= latestX)
                    segment["direction"] = "right";
                else
                    segment["direction"] = "left";
            }
            segment["angle"] = 0;

            if (xMovement > RELOCATION_THRESHOLD || yMovement > RELOCATION_THRESHOLD) {
                currentPathSegment = segment;
                pathArray.append(currentPathSegment);
                zeroAccelCountTemp = 0;
                latestX = positionX;
                latestY = positionY;
                qDebug() << "New movement added to the path.";
                qDebug() << segment << "\n";
            }
        }

        emit positionChanged(positionX, positionY, positionZ);
    }
}

void AccelerometerSensor::applyDenoising()
{
    qreal rawX = m_reading->x() - initialX;
    qreal rawY = m_reading->y() - initialY;
    qreal rawZ = m_reading->z() - initialZ;

    qreal currentX = qAbs(rawX) > threshold ? rawX : 0;
    qreal currentY = qAbs(rawY) > threshold ? rawY : 0;
    qreal currentZ = qAbs(rawZ) > threshold ? rawZ : 0;

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
    traverseAndCleanPathArray();
}

void AccelerometerSensor::addRotationData(int degrees) {
    if (capturing) {
        currentPathSegment["direction"] = degrees > 0 ? "left" : "right";
        currentPathSegment["angle"] = degrees;
        pathArray.append(currentPathSegment);
        qDebug() << "New rotation added to the path.";
    }
}

void AccelerometerSensor::saveDataToJson(const QString &filename) {
    QJsonObject rootObj;
    rootObj["path"] = pathArray;
    QJsonDocument doc(rootObj);

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

QJsonArray AccelerometerSensor::getPathArray()
{
    return pathArray;
}

void AccelerometerSensor::traverseAndCleanPathArray()
{
    if (pathArray.size() < 3) {
        return; // Not enough elements to compare
    }

    for (int i = 1; i < pathArray.size() - 1; ++i) {
        QJsonObject prevSegment = pathArray[i - 1].toObject();
        QJsonObject currentSegment = pathArray[i].toObject();
        QJsonObject nextSegment = pathArray[i + 1].toObject();

        int prevAngle = prevSegment["angle"].toInt();
        int currentAngle = currentSegment["angle"].toInt();
        int nextAngle = nextSegment["angle"].toInt();

        if (prevAngle == nextAngle && prevAngle != 0) {
            pathArray.removeAt(i);
            --i; // Adjust index to account for removed element
        }
    }
}

void AccelerometerSensor::startCalibration()
{
    isCalibrating = true;
    if (!capturing){
        startCapturing();
    }
    calibrationXValues.clear();
    calibrationYValues.clear();
    calibrationZValues.clear();
    qDebug() << "Calibration started.";
}

void AccelerometerSensor::stopCalibration()
{
    if (isCalibrating) {
        calculateMedianCalibration();
        if (capturing){
        stopCapturing();
        }
        isCalibrating = false;
        isCalibrated = true;
        qDebug() << "Calibration stopped and median values calculated.";
    } else {
        qDebug() << "Calibration was not running.";
    }
}

void AccelerometerSensor::calculateMedianCalibration()
{
    if (calibrationXValues.size() == 0 || calibrationYValues.size() == 0 || calibrationZValues.size() == 0) {
        qDebug() << "No data collected for calibration.";
        return;
    }

    std::sort(calibrationXValues.begin(), calibrationXValues.end());
    std::sort(calibrationYValues.begin(), calibrationYValues.end());
    std::sort(calibrationZValues.begin(), calibrationZValues.end());

    int midIndex = calibrationXValues.size() / 2;

    initialX = calibrationXValues[midIndex];
    initialY = calibrationYValues[midIndex];
    initialZ = calibrationZValues[midIndex];

    qDebug() << "Calibration done - Median X:" << initialX << " Y:" << initialY << " Z:" << initialZ;
}
