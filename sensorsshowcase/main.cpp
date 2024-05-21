// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "accelerometersensor.h"
#include "gyroscopesensor.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc,argv);
    qDebug() << "Registering sensor \n";
    qmlRegisterType<AccelerometerSensor>("Sensors", 1, 0, "AccelerometerSensor");
    AccelerometerSensor accelerometerSensor;
    GyroscopeSensor     gyroscopeSensor;
    QGuiApplication::setOrganizationName("QtProject");
    QGuiApplication::setApplicationName("Sensors Showcase");

    QQmlApplicationEngine engine;

    engine.loadFromModule("SensorShowcaseModule", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
