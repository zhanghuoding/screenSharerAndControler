#include "myMedia.h"

MyMedia::MyMedia(QObject *parent) : QObject(parent)
{
#ifdef DESKTOP
    file=new QString("/home/zhanghuoding/screenPic/");

    desk = QApplication::desktop();
    screen = QGuiApplication::primaryScreen();
#endif

#ifdef ANDROID
    file=new QString(appSdcardPath + screenshotPath);
    screen=new QScreenRecorder();
#endif
}
MyMedia::~MyMedia()
{
    if(file)
    {
        file->~QString();
        file = NULL;
    }
#ifdef DESKTOP
    desk = NULL;
    screen = NULL;
#endif

#ifdef ANDROID
    if(screen)
    {
        screen->~QScreenRecorder();
        screen = NULL;
    }
#endif
    //QObject::~QObject();
}
QByteArray* MyMedia::getNextScreenShot()
{
    QByteArray *pic_buffer = new QByteArray();
    QBuffer *buffer = new QBuffer(pic_buffer);
    buffer->open(QIODevice::WriteOnly);

    int num = 0;

    if(!screen)
    {
        qDebug()<<"Pointer create failed!{ From:MyMedia::getNextScreenShot() }";
        return NULL;
    }
    if(!screen->grabWindow(0).save(buffer,"JPEG"))
    //qDebug()<<screen->grabWindow(0).size()<<"TTTTTTTTTTTTTT";
    //if(!screen->grabWindow(0).save(*file + screenshot_PrefixName + QString::number(num++) + screenshot_SuffixName,"JPEG"))
    {
        qDebug()<<"Get screenshot failed!" + Tool::timeSign()<<"{ From:MyMedia::getNextScreenShot() }";
    }

    //QPushButton p;
    //p.setIcon(QIcon(*pic_buffer));
    //p.setIcon(QIcon(*file + screenshot_PrefixName + QString::number(num-1) + screenshot_SuffixName));
    //p.setIcon(QIcon("/mnt/Work/Programs/Qt/screenSharerAndControler/screenSharerAndControler/resource/image/icon.png"));
    //p.show();

    return pic_buffer;
}
