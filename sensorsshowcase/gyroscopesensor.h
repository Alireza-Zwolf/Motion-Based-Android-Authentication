#ifndef GYROSCOPESENSOR_H
#define GYROSCOPESENSOR_H

#include <QObject>
#include <QGyroscope>
#include <QQueue>
#include <QElapsedTimer>
#include <QVector>

class AccelerometerSensor;

class GyroscopeSensor : public QObject
{
    Q_OBJECT
public:
    explicit GyroscopeSensor(AccelerometerSensor *accelerometer, QObject *parent = nullptr);
    ~GyroscopeSensor();

    void setThreshold(qreal newThreshold);
    void setDataRate(int rate);
    Q_INVOKABLE void startCapturing();
    Q_INVOKABLE void stopCapturing();
    Q_INVOKABLE void startCalibration(); // Start collecting data for calibration
    Q_INVOKABLE void stopCalibration(); // Stop collecting data and apply calibration

signals:
    void rotationDetected(int angle);

private slots:
    void updateReading();

private:
    void applyDenoising();
    void detectRotation(qreal adjustedX, qreal adjustedY, qreal adjustedZ);
    void addResultToQueue(int result);
    void calculateMedianCalibration();

    QGyroscope *m_sensor;
    QGyroscopeReading *m_reading;
    bool isCalibrated;
    qreal initialX, initialY, initialZ;
    qreal threshold;
    int windowSize;
    qreal xSum, ySum, zSum;
    qreal accumulatedX, accumulatedY, accumulatedZ;
    int resultSum;
    QQueue<qreal> xQueue, yQueue, zQueue;
    QQueue<int> resultQueue;
    AccelerometerSensor *accelerometerSensor;
    QElapsedTimer timer;
    qint64 lastUpdateTime;
    bool capturing;

    // Calibration variables
    bool isCalibrating;
    QVector<qreal> calibrationXValues;
    QVector<qreal> calibrationYValues;
    QVector<qreal> calibrationZValues;
};

#endif // GYROSCOPESENSOR_H
