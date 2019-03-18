#ifndef CONNECTBYPARTNERHOSTANDSHARESCREEN_H
#define CONNECTBYPARTNERHOSTANDSHARESCREEN_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QGuiApplication>
#include <QScreen>
#include <QBuffer>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include "variable.h"
#include "tool.h"
#include "QScreenRecorder/myMedia.h"

class ConnectByPartnerHostAndShareScreen : public QObject
{
    Q_OBJECT
public:
    explicit ConnectByPartnerHostAndShareScreen(QObject *parent = nullptr);
private:
    QThread *currentThread;
    //QTcpSocket *conTCP;
    QUdpSocket *conUDP;

    MyMedia *mMedia;

    int listenUDPPortAndWaitMessage();
    void connectPartnerForwardly_WhenWeWererequested();

    void getOurScreenPicture();
signals:
    void networkError(QString err,int no = REQUEST_UNKOWN_ERROR);
    void replyServer_WeHaveSendMessageToPartner(QByteArray *send_buffer);

    void preparOurScreenPicture();
    void sendScreenData(QByteArray *send_buffer);
public slots:
    void doWork(QThread *self);
    void readUDPData();
    void readError(QAbstractSocket::SocketError);
};

#endif // CONNECTBYPARTNERHOSTANDSHARESCREEN_H
