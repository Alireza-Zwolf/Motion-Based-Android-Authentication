#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <motionbasedauthentication.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<MotionBasedAuthentication> ("MotionBasedAuthentication", 1, 0, "MotionBasedAuthentication");

    const QUrl url(u"qrc:/Motion-Based-Authentication-1/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);
    if(engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
