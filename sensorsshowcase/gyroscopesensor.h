#ifndef GYROSCOPESENSOR_H
#define GYROSCOPESENSOR_H

#include <QObject>
#include <QGyroscope>
#include <QQueue>
#include <QElapsedTimer>

class AccelerometerSensor;

class GyroscopeSensor : public QObject
{
    Q_OBJECT
public:
    explicit GyroscopeSensor(AccelerometerSensor *accelerometer, QObject *parent = nullptr);
    ~GyroscopeSensor();

    void calibrate();
    void setThreshold(qreal newThreshold);
    void setDataRate(int rate);
    Q_INVOKABLE  void startCapturing();
    Q_INVOKABLE  void stopCapturing();

signals:
    void rotationDetected(int angle);

private slots:
    void updateReading();

private:
    void applyDenoising();
    void detectRotation(qreal adjustedX, qreal adjustedY, qreal adjustedZ);
    void addResultToQueue(int result);

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
};

#endif // GYROSCOPESENSOR_H
