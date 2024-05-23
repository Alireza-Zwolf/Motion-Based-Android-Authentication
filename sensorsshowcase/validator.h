#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "qqmlintegration.h"
#include <QObject>
#include <QJsonArray>

class AccelerometerSensor;

class Validator : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Validator(AccelerometerSensor *accelerometer, QObject *parent = nullptr);

    QJsonArray createCorrectSample();
    QJsonObject createSegment(QJsonObject start, QJsonObject end, QString direction, QString angle);
    bool comparePaths(const QJsonArray &capturedPath, const QJsonArray &correctPath, qreal threshold);

signals:
    void validationResult(bool result);

public slots:
    void validatePath();

private:
    bool isWithinThreshold(const QJsonObject &point1, const QJsonObject &point2, qreal threshold, const QString &axis);

    AccelerometerSensor *accelerometerSensor;
};

#endif // VALIDATOR_H
