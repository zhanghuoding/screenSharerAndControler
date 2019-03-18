#ifndef QSCREENRECORDER_H
#define QSCREENRECORDER_H

#ifdef ANDROID

#include <QObject>
#include <QtAndroidExtras/QtAndroidExtras>
#include <QByteArray>
#include <QPixmap>
#include <QMessageBox>

#include "variable.h"

#define CHECK_EXCEPTION() \
    if(env->ExceptionCheck())\
    {\
        qDebug() << "exception occured";\
        env->ExceptionClear();\
    }

using namespace QtAndroid;

class QScreenRecorder : public QObject
{
    Q_OBJECT
public:
    explicit QScreenRecorder(QObject *parent = nullptr);
    ~QScreenRecorder();

    QByteArray *getImageByteArray();
    int getDataLength();
    bool isScreenPicFinish();

    static QPixmap grabWindow(WId window, int x = 0, int y = 0, int w = -1, int h = -1);

private:
    QByteArray *imageByte;
    int dataLength;
    bool screenPicHasBeenShot;

    static int requestId;

    QAndroidJniObject *mMediaProjectionManager;
    QAndroidJniObject *mContext;
};

#endif //ANDROID

#endif // QSCREENRECORDER_H
