#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "accelerometersensor.h"
#include "gyroscopesensor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<AccelerometerSensor>("Sensors", 1, 0, "AccelerometerSensor");
    qmlRegisterType<GyroscopeSensor>("Sensors", 1, 0, "GyroscopeSensor");


    AccelerometerSensor accelerometerSensor;
    GyroscopeSensor gyroscopeSensor(&accelerometerSensor);

    QQmlApplicationEngine engine;

    QObject::connect(&gyroscopeSensor, &GyroscopeSensor::rotationDetected, &accelerometerSensor, &AccelerometerSensor::addRotationData);

    engine.rootContext()->setContextProperty("accelerometerSensor", &accelerometerSensor);
    engine.rootContext()->setContextProperty("gyroscopeSensor", &gyroscopeSensor);

    engine.loadFromModule("SensorShowcaseModule", "Main");;
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
