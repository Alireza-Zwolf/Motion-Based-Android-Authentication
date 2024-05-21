#ifndef GYROSCOPESENSOR_H
#define GYROSCOPESENSOR_H

#include <QObject>
#include <QGyroscope>
#include <QGyroscopeReading>
#include <QQueue>

class GyroscopeSensor : public QObject
{
    Q_OBJECT

public:
    explicit GyroscopeSensor(QObject *parent = nullptr);
    ~GyroscopeSensor();

    void calibrate();
    void setThreshold(qreal threshold);
    void setDataRate(int rate);

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

};

#endif // GYROSCOPESENSOR_H
