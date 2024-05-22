#ifndef ACCELEROMETERSENSOR_H
#define ACCELEROMETERSENSOR_H

<<<<<<< HEAD
=======
#include "qjsonarray.h"
#include "qjsonobject.h"
>>>>>>> 0e044446bf3d99f979576ddfab882cbba6682001
#include <QObject>
#include <QAccelerometer>
#include <QAccelerometerReading>
#include <QQueue>
#include <QElapsedTimer>

class AccelerometerSensor : public QObject
{
    Q_OBJECT

public:
    explicit AccelerometerSensor(QObject *parent = nullptr);
    ~AccelerometerSensor();

    void calibrate(); // Method to calibrate the sensor
    void setThreshold(qreal threshold); // Set the motion detection threshold

<<<<<<< HEAD
=======
    Q_INVOKABLE void startCapturing();
    Q_INVOKABLE void stopCapturing();
    void addRotationData(int degrees);
    Q_INVOKABLE void saveDataToJson(const QString &filename);

>>>>>>> 0e044446bf3d99f979576ddfab882cbba6682001
signals:
    void positionChanged(qreal x, qreal y, qreal z);

private slots:
    void updateReading();

private:
    void applyDenoising(); // Apply denoising to the current reading

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
<<<<<<< HEAD

    QElapsedTimer lastUpdateTime; // Timer to measure elapsed time between updates
    int zeroAccelCount; // Counter for consecutive zero accelerations
=======
    qreal latestX; // Current position in X direction
    qreal latestY; // Current position in Y direction
    qreal latestZ; // Current position in Z direction

    QElapsedTimer lastUpdateTime; // Timer to measure elapsed time between updates
    int zeroAccelCount; // Counter for consecutive zero accelerations
    int zeroAccelCountTemp; // Counter for consecutive zero accelerations for saving data into a .json file


    bool capturing;
    QJsonArray pathArray;
    QJsonObject currentPathSegment;
>>>>>>> 0e044446bf3d99f979576ddfab882cbba6682001
};

#endif // ACCELEROMETERSENSOR_H
