#ifndef ACCELEROMETERSENSOR_H
#define ACCELEROMETERSENSOR_H

#include "qjsonarray.h"
#include "qjsonobject.h"
#include <QObject>
#include <QAccelerometer>
#include <QAccelerometerReading>
#include <QQueue>
#include <QElapsedTimer>
#include <QVector>

class AccelerometerSensor : public QObject
{
    Q_OBJECT

public:
    explicit AccelerometerSensor(QObject *parent = nullptr);
    ~AccelerometerSensor();

    void setThreshold(qreal threshold); // Set the motion detection threshold

    Q_INVOKABLE void startCapturing();
    Q_INVOKABLE void stopCapturing();
    Q_INVOKABLE void startCalibration(); // Start collecting data for calibration
    Q_INVOKABLE void stopCalibration(); // Stop collecting data and apply calibration
    void addRotationData(int degrees);
    Q_INVOKABLE void saveDataToJson(const QString &filename);
    Q_INVOKABLE  QJsonArray getPathArray();
    void traverseAndCleanPathArray();

signals:
    void positionChanged(qreal x, qreal y, qreal z);

private slots:
    void updateReading();

private:
    void applyDenoising(); // Apply denoising to the current reading
    void calculateMedianCalibration();

    QAccelerometer *m_sensor;
    QAccelerometerReading *m_reading;

    qreal initialX;
    qreal initialY;
    qreal initialZ;

    qreal threshold; // Threshold for motion detection

    bool isCalibrated;

    QQueue<qreal> xQueue; // Queues for moving average filter
    QQueue<qreal> yQueue;
    QQueue<qreal> zQueue;
    int windowSize; // Window size for the moving average filter
    qreal xSum;
    qreal ySum;
    qreal zSum;

    qreal velocityX; // Current velocity in X direction
    qreal velocityY; // Current velocity in Y direction
    qreal velocityZ; // Current velocity in Z direction
    qreal positionX; // Current position in X direction
    qreal positionY; // Current position in Y direction
    qreal positionZ; // Current position in Z direction
    qreal latestX; // Latest position in X direction
    qreal latestY; // Latest position in Y direction
    qreal latestZ; // Latest position in Z direction

    QElapsedTimer lastUpdateTime; // Timer to measure elapsed time between updates
    int zeroAccelCount; // Counter for consecutive zero accelerations
    int zeroAccelCountTemp; // Counter for consecutive zero accelerations for saving data into a .json file

    bool capturing;
    QJsonArray pathArray;
    QJsonObject currentPathSegment;

    // Calibration variables
    bool isCalibrating;
    QVector<qreal> calibrationXValues;
    QVector<qreal> calibrationYValues;
    QVector<qreal> calibrationZValues;
};

#endif // ACCELEROMETERSENSOR_H
