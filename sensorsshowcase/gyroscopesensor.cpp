#include "gyroscopesensor.h"
#include "accelerometersensor.h"
#include <QDebug>
#include <QtMath>
#include <QElapsedTimer>
#include <algorithm>

const qreal DEFAULT_THRESHOLD = 0;  // Default threshold
const int DEFAULT_WINDOW_SIZE = 1; // Default window size for moving average
const int AVERAGE_WINDOW_SIZE = 1; // Number of results to average

GyroscopeSensor::GyroscopeSensor(AccelerometerSensor *accelerometer, QObject *parent)
    : QObject(parent),
    m_sensor(new QGyroscope(this)),
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
    accumulatedX(0.0),
    accumulatedY(0.0),
    accumulatedZ(0.0),
    resultSum(0),
    accelerometerSensor(accelerometer),
    lastUpdateTime(0),
    isCalibrating(false)
{
    connect(m_sensor, &QGyroscope::readingChanged, this, &GyroscopeSensor::updateReading);
    m_sensor->start();
    timer.start(); // Start the elapsed timer
}

GyroscopeSensor::~GyroscopeSensor()
{
    m_sensor->stop();
}

void GyroscopeSensor::setThreshold(qreal newThreshold)
{
    threshold = newThreshold;
}

void GyroscopeSensor::setDataRate(int rate)
{
    if (rate > 0) {
        int dataRateMicroseconds = 1000000 / rate;
        m_sensor->setDataRate(dataRateMicroseconds);
        qDebug() << "Data rate set to" << rate << "Hz (" << dataRateMicroseconds << " microseconds)";
    } else {
        qDebug() << "Invalid data rate. Must be greater than 0.";
    }
}

void GyroscopeSensor::updateReading()
{
    m_reading = m_sensor->reading();
    if (m_reading) {
        if (isCalibrating) {
            calibrationXValues.append(m_reading->x());
            calibrationYValues.append(m_reading->y());
            calibrationZValues.append(m_reading->z());
        }

        qint64 currentTime = timer.elapsed();
        qreal timeInterval = (currentTime - lastUpdateTime) / 1000.0; // Convert to seconds
        lastUpdateTime = currentTime;

        // Apply denoising
        applyDenoising();

        // Adjusted readings
        qreal adjustedX = m_reading->x() - initialX;
        qreal adjustedY = m_reading->y() - initialY;
        qreal adjustedZ = m_reading->z() - initialZ;

        // Calculate angular changes
        qreal angularChangeX = adjustedX * timeInterval;
        qreal angularChangeY = adjustedY * timeInterval;
        qreal angularChangeZ = adjustedZ * timeInterval;

        // Accumulate the angular changes
        accumulatedX += angularChangeX;
        accumulatedY += angularChangeY;
        accumulatedZ += angularChangeZ;

        // Detect 90-degree rotations
        detectRotation(accumulatedX, accumulatedY, accumulatedZ);
    }
}

void GyroscopeSensor::applyDenoising()
{
    qreal currentX = m_reading->x();
    qreal currentY = m_reading->y();
    qreal currentZ = m_reading->z();

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

void GyroscopeSensor::detectRotation(qreal adjustedX, qreal adjustedY, qreal adjustedZ)
{
    if (qFabs(adjustedZ) >= 85) {
        addResultToQueue(adjustedZ > 0 ? 90 : -90);
        accumulatedZ = 0;
    }
}

void GyroscopeSensor::addResultToQueue(int result)
{
    resultQueue.enqueue(result);
    resultSum += result;

    if (resultQueue.size() >= AVERAGE_WINDOW_SIZE) {
        int averageResult = resultSum / resultQueue.size();
        if (averageResult == 90 || averageResult == -90) {
            qDebug() << "Final 90 Degree Detection Result ↩️" << averageResult;
            accelerometerSensor->addRotationData(averageResult);
        }
        resultQueue.clear();
        resultSum = 0;
    }
}

void GyroscopeSensor::startCapturing() {
    capturing = true;
}

void GyroscopeSensor::stopCapturing() {
    capturing = false;
}

void GyroscopeSensor::startCalibration()
{
    isCalibrating = true;
    calibrationXValues.clear();
    calibrationYValues.clear();
    calibrationZValues.clear();
    qDebug() << "Calibration started.";
}

void GyroscopeSensor::stopCalibration()
{
    if (isCalibrating) {
        calculateMedianCalibration();
        isCalibrating = false;
        isCalibrated = true;
        qDebug() << "Calibration stopped and median values calculated.";
    } else {
        qDebug() << "Calibration was not running.";
    }
}

void GyroscopeSensor::calculateMedianCalibration()
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
