#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "accelerometersensor.h"
#include "gyroscopesensor.h"
#include "validator.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<AccelerometerSensor>("Sensors", 1, 0, "AccelerometerSensor");
    qmlRegisterType<GyroscopeSensor>("Sensors", 1, 0, "GyroscopeSensor");
    qmlRegisterType<Validator>("Sensors", 1, 0, "Validator");

    AccelerometerSensor accelerometerSensor;
    GyroscopeSensor gyroscopeSensor(&accelerometerSensor);
    gyroscopeSensor.setDataRate(100);

    Validator validator(&accelerometerSensor);

    QQmlApplicationEngine engine;

    QObject::connect(&gyroscopeSensor, &GyroscopeSensor::rotationDetected, &accelerometerSensor, &AccelerometerSensor::addRotationData);

    engine.rootContext()->setContextProperty("accelerometerSensor", &accelerometerSensor);
    engine.rootContext()->setContextProperty("gyroscopeSensor", &gyroscopeSensor);
    engine.rootContext()->setContextProperty("validator", &validator);

    engine.loadFromModule("SensorShowcaseModule", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
