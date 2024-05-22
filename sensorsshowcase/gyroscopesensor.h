#ifndef GYROSCOPESENSOR_H
#define GYROSCOPESENSOR_H

#include <QObject>
#include <QGyroscope>
#include <QGyroscopeReading>
#include <QQueue>
#include <accelerometersensor.h>

class GyroscopeSensor : public QObject
{
    Q_OBJECT

public:
    explicit GyroscopeSensor(AccelerometerSensor *accelerometer, QObject *parent = nullptr);
    ~GyroscopeSensor();

    void calibrate();
    void setThreshold(qreal threshold);
    void setDataRate(int rate);

    Q_INVOKABLE void startCapturing();
    Q_INVOKABLE void stopCapturing();

signals:
    void rotationDetected(int degrees);

private slots:
    void updateReading();

private:
    void applyDenoising();
    void detectRotation(qreal adjustedX, qreal adjustedY, qreal adjustedZ);
    void addResultToQueue(int result);

    QGyroscope *m_sensor;
    QGyroscopeReading *m_reading;

    qreal initialX;
    qreal initialY;
    qreal initialZ;

    qreal threshold;

    bool isCalibrated;

    QQueue<qreal> xQueue;
    QQueue<qreal> yQueue;
    QQueue<qreal> zQueue;
    int windowSize;
    qreal xSum;
    qreal ySum;
    qreal zSum;

    qreal accumulatedX;
    qreal accumulatedY;
    qreal accumulatedZ;

    QQueue<int> resultQueue;
    int resultSum;

    bool capturing;
    AccelerometerSensor *accelerometerSensor;

};

#endif // GYROSCOPESENSOR_H
