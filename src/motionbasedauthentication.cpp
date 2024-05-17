#include "motionbasedauthentication.h"

MotionBasedAuthentication::MotionBasedAuthentication(QObject *parent): QObject(parent)
{

}

void MotionBasedAuthentication::startRecording() {
    // to do
}

void MotionBasedAuthentication::endRecording() {
    // to do
}

void MotionBasedAuthentication::authenticate() {
    // To do

    result = true;
    resultChanged(result);
}

void MotionBasedAuthentication::showData() {
    // To do

    data = "Here's your data";
    dataChanged(data);
}
