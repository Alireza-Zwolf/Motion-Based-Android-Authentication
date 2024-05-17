#ifndef MOTIONBASEDAUTHENTICATION_H
#define MOTIONBASEDAUTHENTICATION_H

#include <QObject>
#include <QDebug>

class MotionBasedAuthentication: public QObject
{
    Q_OBJECT

public:
    explicit MotionBasedAuthentication(QObject *parent = nullptr);

    Q_PROPERTY(bool result NOTIFY resultChanged FINAL)
    Q_PROPERTY(QString data NOTIFY dataChanged FINAL)

signals:
    void resultChanged(bool result);
    void dataChanged(QString data);

public slots:
    void startRecording();
    void endRecording();
    void authenticate();
    void showData();

private:
    bool result;
    QString data;
};

#endif // MOTIONBASEDAUTHENTICATION_H
