#ifndef QSCREENRECORDERRESULTRECEIVER_H
#define QSCREENRECORDERRESULTRECEIVER_H

#ifdef ANDROID_

#include <QObject>
#include <QtAndroidExtras>
#include <QByteArray>

#include "variable.h"

#define CHECK_EXCEPTION() \
    if(env->ExceptionCheck())\
    {\
    qDebug() << "exception occured";\
    env->ExceptionClear();\
    }

using namespace QtAndroid;

class QScreenRecorderResultReceiver:public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    explicit QScreenRecorderResultReceiver(int requestId);
    ~QScreenRecorderResultReceiver();

    static int requestId;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject & data);

    QByteArray *getImageByteArray();
    int getDataLength();
    bool isScreenPicFinish();

private:
    QByteArray *imageByte;
    int dataLength;
    bool screenPicHasBeenShot;

};

#endif //ANDROID

#endif // QSCREENRECORDERRESULTRECEIVER_H
