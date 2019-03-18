#include "QScreenRecorder/qScreenRecorder.h"

#ifdef ANDROID

int QScreenRecorder::requestId = 1;

QScreenRecorder::QScreenRecorder(QObject *parent) : QObject(parent)
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity=androidActivity();
    CHECK_EXCEPTION()

    mMediaProjectionManager = NULL;

    if(androidSdkVersion() >= 21)
    {
        mContext = new QAndroidJniObject("android/content/Context","()V");
        *mContext = androidContext();
        mMediaProjectionManager = new QAndroidJniObject("android/media/projection/MediaProjectionManager",\
                                                        "()V");
        //startActivityForResult(\
                        ((MediaProjectionManager) getSystemService("media_projection")).createScreenCaptureIntent(),\
                        1);
        *mMediaProjectionManager = mContext->callObjectMethod("getSystemService",\
                                                            "(Ljava/lang/String;)Ljava/lang/Object;",\
                                                              mContext->getObjectField("MEDIA_PROJECTION_SERVICE","Ljava/lang/String;").object<jstring>());

        qDebug()<<"GGGGGGGGGGGGGGGGGGGGGGGG"<<mContext->isValid();
        qDebug()<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<mContext->getObjectField("MEDIA_PROJECTION_SERVICE","Ljava/lang/String;").toString();
    }else
    {
        QMessageBox::critical(0, QObject::tr("About SDK version:"),QObject::tr("The version of current android SDK is too low to shoot screen!"));
        qDebug()<<"The version of current android SDK is too low to shoot screen!";
    }


    screenPicHasBeenShot = false;
    imageByte=new QByteArray;
    imageByte->resize(BUFFER_SIZE);
    dataLength=0;
}
QScreenRecorder::~QScreenRecorder()
{

}
QPixmap QScreenRecorder::grabWindow(WId window, int x, int y, int w, int h)
{
    QPixmap ret;
    return ret;
}
QByteArray *QScreenRecorder::getImageByteArray()
{
    return imageByte;
}
int QScreenRecorder::getDataLength()
{
    return dataLength;
}
bool QScreenRecorder::isScreenPicFinish()
{
    return screenPicHasBeenShot;
}

#endif //ANDROID
