#ifndef TCPCONNECTERWITHSERVER_H
#define TCPCONNECTERWITHSERVER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include "variable.h"
#include "tool.h"

class TcpConnecterWithServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpConnecterWithServer(QObject *parent = nullptr);
private:
    QThread *currentThread;
    QTcpSocket *conTCP;
    //QTcpServer *listenPartnerAndServerTCP;

    void deal_con_ret(QString recv_buffer_orig);
    void deal_req_ret(QString recv_buffer_orig);
    void deal_dis_ret(QString recv_buffer_orig);
    void requestedHostNotExists();
    void requestedYourSelf();

    void keepalive(QString recv_buffer_orig);
    void connectPartner_WhenWereRequested(QString recv_buffer_orig);
    void afterSendShareScreenRequests(QString recv_buffer_orig);
    void weHaveReceivedScreenData(QByteArray recv_buffer_orig);
    void partnerWillExitAPP(QString recv_buffer_orig);

signals:
    void networkError(QString err,int no = REQUEST_UNKOWN_ERROR);
    void userID_Passwd_Changed(QString iD,QString passwd);
    void weWereRequested();
    void startForwardly_ConnctPartnerHost();
    void OK_exit(int flag);

    void preparTowShowScreenPicture();
    void showScreenShotReceived(QByteArray recv_byte_buffer);

    void yourPartnerHaveExit();
    void serverFinalPortChanged();
public slots:
    void doWork(QThread *self);
    int registerHost();
    void readTCPData();
    void readError(QAbstractSocket::SocketError);
    void requestPartnerHostInfo(QByteArray send_buffer);
    void replyServer_WeHaveSendMessageToPartner(QByteArray *send_buffer_byte);
    void sendDisconnectRequest();
    void sendKeepAliveRequest();

    void sendScreenData(QByteArray *send_buffer);
};

#endif // TCPCONNECTERWITHSERVER_H
