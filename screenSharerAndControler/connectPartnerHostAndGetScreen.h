#ifndef CONNECTPARTNERHOSTANDGETSCREEN_H
#define CONNECTPARTNERHOSTANDGETSCREEN_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QUdpSocket>
#include "variable.h"
#include "tool.h"


class ConnectPartnerHostAndGetScreen : public QObject
{
    Q_OBJECT
public:
    explicit ConnectPartnerHostAndGetScreen(QObject *parent = nullptr);
private:
    QThread *currentThread;
    QUdpSocket *conUDP;

    int reqPartnerHostInfo();

signals:
    void networkError(QString err,int no = REQUEST_UNKOWN_ERROR);
    void requestPartnerHostInfo(QByteArray send_buffer);
public slots:
    void doWork(QThread *self);
    void readUDPData();
    void readError(QAbstractSocket::SocketError);
    void startForwardly_ConnctPartnerHost();
};

#endif // CONNECTPARTNERHOSTANDGETSCREEN_H
