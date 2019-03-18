#ifndef MYMEDIA_H
#define MYMEDIA_H

#include <QObject>
#include <QPixmap>
#include <QByteArray>
#include <QScreen>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QApplication>
#include <QBuffer>

#include "variable.h"
#include "tool.h"

#ifdef ANDROID
#include "qScreenRecorder.h"
#endif

class MyMedia : public QObject
{
    Q_OBJECT
public:
    explicit MyMedia(QObject *parent = nullptr);
    ~MyMedia();

    QByteArray* getNextScreenShot();

signals:

public slots:

private:
    QString *file;
#ifdef DESKTOP
    QDesktopWidget *desk;
    QScreen *screen;
#endif

#ifdef ANDROID
    QScreenRecorder *screen;
#endif
};

#endif // MYMEDIA_H
