#include "validator.h"
#include "accelerometersensor.h"
#include <QJsonObject>
#include <QDebug>
#include <QtMath> // For qFabs and qSqrt

const qreal DISTANCE_APPROXIMATION_THRESHOLD = 0.5; // Adjust this value as needed


Validator::Validator(AccelerometerSensor *accelerometer, QObject *parent)
    : QObject(parent), accelerometerSensor(accelerometer)
{
}

QJsonArray Validator::createCorrectSample()
{
    QJsonArray correctPath;

    // QString right_agnel = -90;
    // Create a manual sample of the correct path
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0}}), QJsonObject({{"x", -0.2}, {"y", 0}}), "left", 0));
    correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0}}), QJsonObject({{"x", 0}, {"y", 0.0}}), "left", 90));
    correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0}}), QJsonObject({{"x", 0}, {"y", 0.0}}), "left", 90));
    correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0}}), QJsonObject({{"x", 0}, {"y", -20.0}}), "down", 0));
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", -20.0}}), QJsonObject({{"x", 20.0}, {"y", 0.0}}), "right", 90));
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0}}), QJsonObject({{"x", 0}, {"y", 0.0}}), "right", 90));
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0.2}}), QJsonObject({{"x", 0}, {"y", 0.2}}), "right", -90));
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0.2}}), QJsonObject({{"x", 0}, {"y", 0.2}}), "right", -90));
    // correctPath.append(createSegment(QJsonObject({{"x", 0}, {"y", 0.2}}), QJsonObject({{"x", -0.2}, {"y", 0.2}}), "left", 0));

    // correctPath.append(createSegment(QJsonObject({{"x", -10}, {"y", 10}}), QJsonObject({{"x", 10}, {"y", 10}}), "right", "0"));

    return correctPath;
}

QJsonObject Validator::createSegment(QJsonObject start, QJsonObject end, QString direction, qreal angle)
{
    QJsonObject segment;
    segment["start"] = start;
    segment["end"] = end;
    segment["direction"] = direction;
    segment["angle"] = angle;
    return segment;
}

bool Validator::comparePaths(const QJsonArray &capturedPath, const QJsonArray &correctPath, qreal threshold)
{
    if (capturedPath.size() != correctPath.size()) {
        return false;
    }

    for (int i = 0; i < capturedPath.size(); ++i) {
        QJsonObject capturedSegment = capturedPath[i].toObject();
        QJsonObject correctSegment = correctPath[i].toObject();

        // qDebug() << capturedSegment;
        // qDebug() << correctSegment;
        // Compare direction and angle
        if (capturedSegment["direction"].toString().compare(correctSegment["direction"].toString()) != 0) {
            qDebug() << "False1";
            return false;
        }

        if (capturedSegment["angle"].toInt() != correctSegment["angle"].toInt()) {
            qDebug() << "False11";
            return false;
        }

        // Compare start and end points based on direction
        QString direction = capturedSegment["direction"].toString();
        if ((direction == "left" || direction == "right") &&
            (!isWithinThreshold(capturedSegment["start"].toObject(), correctSegment["start"].toObject(), threshold, "x") ||
             !isWithinThreshold(capturedSegment["end"].toObject(), correctSegment["end"].toObject(), threshold, "x"))) {
            qDebug() << "False2";
            return false;
        }

        if ((direction == "up" || direction == "down") &&
            (!isWithinThreshold(capturedSegment["start"].toObject(), correctSegment["start"].toObject(), threshold, "y") ||
             !isWithinThreshold(capturedSegment["end"].toObject(), correctSegment["end"].toObject(), threshold, "y"))) {
            qDebug() << "False3";
            return false;
        }
    }
    // qDebug() << "Works Well!";
    return true;
}

bool Validator::isWithinThreshold(const QJsonObject &point1, const QJsonObject &point2, qreal threshold, const QString &axis)
{
    qreal value1 = point1[axis].toDouble();
    qreal value2 = point2[axis].toDouble();

    qreal distance = qAbs(value2 - value1);
    return distance <= threshold;
}

void Validator::validatePath()
{
    QJsonArray capturedPath = accelerometerSensor->getPathArray();
    // qDebug() << "validatePath function: capturedPath:";
    // qDebug() << capturedPath;
    QJsonArray correctPath = createCorrectSample();
    // qDebug() << "validatePath function: correctPath:";
    // qDebug() << correctPath;
    bool result = comparePaths(capturedPath, correctPath, DISTANCE_APPROXIMATION_THRESHOLD);
    qDebug() << result;
    emit validationResult(result);
}







