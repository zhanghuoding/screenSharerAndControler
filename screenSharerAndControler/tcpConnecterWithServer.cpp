#include "tcpConnecterWithServer.h"
#include "getLocalHostInfo.h"

TcpConnecterWithServer::TcpConnecterWithServer(QObject *parent) : QObject(parent)
{
    currentThread=NULL;
    conTCP=NULL;
    //listenPartnerAndServerTCP=NULL;
}
void TcpConnecterWithServer::doWork(QThread *self)
{
    currentThread=self;
    GetLocalHostInfo gl;
    gl.getLocalHostIpV4Address(localHostInfo_s);
    gl.getLocalHostMacAddress(localHostInfo_s);

    registerHost();
}
int TcpConnecterWithServer::registerHost()
{
    if(conTCP)
    {
        conTCP->close();
        conTCP->~QTcpSocket();
    }
    conTCP=new QTcpSocket();
    /*if(!conTCP->bind(userLocalPort,QAbstractSocket::ReuseAddressHint|QAbstractSocket::ShareAddress))
    {
        //use share mode bind port
        qDebug()<<"Bind port faild please retry! { From:TcpConnecterWithServer::registerHost() }" + Tool::timeSign();
        return -1;
    }*/
    connect(conTCP, SIGNAL(readyRead()), this, SLOT(readTCPData()));
    connect(conTCP, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readError(QAbstractSocket::SocketError)));
    conTCP->connectToHost(QHostAddress(serverIP),serverFinalPort);
    //conTCP->setSocketOption(QAbstractSocket::KeepAliveOption,1);

    int i=1;
    bool bind_success_flag=false;
    serverFinalPort=serverDefaultPort;
    while(!bind_success_flag && serverFinalPort <= (serverDefaultPort + port_max_difference_value))
    {
        conTCP->connectToHost(QHostAddress(serverIP),serverFinalPort);

        for(i=1;i<=CONNECT_TIMES;i++)
        {//connect for many times
            if (conTCP->waitForConnected(1000)) //success
            {
                bind_success_flag=true;
                break;
            }else
            {
                qDebug()<<"Connect to server faild for the "<<i<<"'s times,and port="<<serverFinalPort<<" { From:TcpConnecterWithServer::registerHost() }";
            }
        }
        if(bind_success_flag)
        {
            emit serverFinalPortChanged();
            break;
        }
        else
        {
            serverFinalPort++;
            emit serverFinalPortChanged();
        }
    }
    if(!bind_success_flag)
    {
        serverFinalPort=serverDefaultPort;
        emit serverFinalPortChanged();
        qDebug()<<"Connect to server faild for all "<<CONNECT_TIMES<<" times and all port,please retry later! { From:TcpConnecterWithServer::registerHost() }";
        return -1;
    }

    /*if(listenPartnerAndServerTCP)
    {
        listenPartnerAndServerTCP->close();
        listenPartnerAndServerTCP->~QTcpServer();
    }
    //listenPartnerAndServerTCP=new QTcpServer();

    //bind this port for connected by partner host
    if(!conTCP->bind(conTCP->localPort(),QTcpSocket::ShareAddress))
    {//use share mode bind port
        qDebug()<<"Bind port faild please retry! { From:TcpConnecterWithServer::registerHost() }";
        return -1;
    }

    //listen this port
    if(!listenPartnerAndServerTCP->listen(QHostAddress::Any,conTCP->localPort()))
    {
        qDebug()<<"Listen port faild please retry! { From:TcpConnecterWithServer::registerHost() }";
        return -1;
    }*/

    QByteArray send_buffer_byte;
    send_buffer_byte.resize(INT_BYTE_LENGTH);
    MY_MACRO_INT nut=CONNECT;
    memcpy(send_buffer_byte.data(),&nut,INT_BYTE_LENGTH);

    QString send_buffer;
    //QString send_buffer(QString::number(CONNECT));
    //send_buffer.append("@");
    int n = localHostInfo_s->numOfLocalHostMac();
    int j = 0;
    for(j=0;j<n;j++)
    {
        send_buffer.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
        send_buffer.append("@");
    }
    qDebug()<<send_buffer + Tool::timeSign()<<"{ From:TcpConnecterWithServer::registerHost() }";
    send_buffer_byte += send_buffer.toLatin1();
    conTCP->write(send_buffer_byte);

    conTCP->setReadBufferSize(BUFFER_SIZE);

    //record local port
    userLocalPort = conTCP->localPort();
}
void TcpConnecterWithServer::readTCPData()
{
    QByteArray recv_buffer_byte=conTCP->readAll();
    //qDebug()<<recv_buffer <<"{ From:TcpConnecterWithServer::readTCPData() }";

    MY_MACRO_INT fl;
    memcpy(&fl,recv_buffer_byte.data(),INT_BYTE_LENGTH);
//qDebug()<<"MMMMMMMMMMMMMMMM"<<fl<<"GGGGGGGGG"<<recv_buffer_byte.size().   { From : TcpConnecterWithServer::readTCPData()};
    switch(fl)
    {
    case CONNECT:
        deal_con_ret(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case REQUEST_PARTNER_HOST_INFO:
        deal_req_ret(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case DISCONNECT:
        deal_dis_ret(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case KEEP_ALIVE_SIGNAL:
        keepalive(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case YOU_WARE_REQUESTED_TO_BE_CONNECTED_BY_THIS_HOST:
        connectPartner_WhenWereRequested(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case REQUEST_TO_SHARE_SCREEN:
        afterSendShareScreenRequests(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    case THIS_IS_PICTURE_DATA:
        weHaveReceivedScreenData(QByteArray(recv_buffer_byte));
        break;
    case I_WILL_EXIT://partner will exit app
        partnerWillExitAPP(QString(recv_buffer_byte.data() + INT_BYTE_LENGTH));
        break;
    default:
        weHaveReceivedScreenData(QByteArray(recv_buffer_byte));
        break;
    }
}
void TcpConnecterWithServer::readError(QAbstractSocket::SocketError socketE)
{
    ;
}
void TcpConnecterWithServer::deal_con_ret(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUESTED_HOST_NOT_EXISTS:
        break;
    case REQUESTED_YOUR_SELF:
        break;
    case REQUEST_SUCCESS:
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        userId=recv_buffer.left(recv_buffer.indexOf("@"));
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        userPasswd=recv_buffer.left(recv_buffer.indexOf("@"));
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        selfIP==recv_buffer.left(recv_buffer.indexOf("@"));
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        selfPort=recv_buffer.left(recv_buffer.indexOf("@")).toInt();

        emit userID_Passwd_Changed(userId,userPasswd);
        qDebug()<<"User's ID and passwd have been updated! { From:TcpConnecterWithServer::deal_con_ret(QString recv_buffer_orig) }";

        sendKeepAliveRequest();
        break;
    case REQUEST_UNKOWN_ERROR:
        break;
    case RECEIVED_STRING_TOO_SHORT:
        break;
    default:
        break;
    }
}
void TcpConnecterWithServer::deal_dis_ret(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUESTED_HOST_NOT_EXISTS:
        break;
    case REQUESTED_YOUR_SELF:
        break;
    case REQUEST_SUCCESS:
        emit OK_exit(1);
        break;
    case REQUEST_UNKOWN_ERROR:
        break;
    case RECEIVED_STRING_TOO_SHORT:
        break;
    default:
        break;
    }
}
void TcpConnecterWithServer::deal_req_ret(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUESTED_HOST_NOT_EXISTS:
        requestedHostNotExists();
        break;
    case REQUESTED_YOUR_SELF:
        requestedYourSelf();
        break;
    case REQUEST_SUCCESS:
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        partnerHostIP=recv_buffer.left(recv_buffer.indexOf("@"));
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        partnerHostPort=recv_buffer.left(recv_buffer.indexOf("@")).toInt();
        qDebug()<<"Partner host's IP and port have got! { From:ConnectPartnerHostAndGetScreen::deal_req_ret(QString recv_buffer_orig) }";

        //connect Partner host
        //emit startForwardly_ConnctPartnerHost();

        //then keep this connection alive
        sendKeepAliveRequest();
        break;
    case REQUEST_UNKOWN_ERROR:
        break;
    case RECEIVED_STRING_TOO_SHORT:
        break;
    default:
        break;
    }
}
void TcpConnecterWithServer::keepalive(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    //comunicate with server to keep connection alive
    qDebug()<<recv_buffer_orig + "{ From:TcpConnecterWithServer::keepalive(QString recv_buffer_orig) }";

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUESTED_HOST_NOT_EXISTS:
        break;
    case REQUESTED_YOUR_SELF:
        break;
    case REQUEST_SUCCESS:
        break;
    case REQUEST_UNKOWN_ERROR:
        break;
    case RECEIVED_STRING_TOO_SHORT:
        break;
    default:
        break;
    }

    sendKeepAliveRequest();
}
void TcpConnecterWithServer::requestPartnerHostInfo(QByteArray send_buffer)
{
    if(!conTCP)
    {
        QMessageBox::critical(0, QObject::tr("TCP连接错误："),QObject::tr("请求伙伴信息时的TCP对象已被销毁！"));
        return;
    }
    conTCP->write(send_buffer);
}
void TcpConnecterWithServer::connectPartner_WhenWereRequested(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUESTED_HOST_NOT_EXISTS:
        break;
    case REQUESTED_YOUR_SELF:
        break;
    case REQUEST_SUCCESS:
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        partnerHostIP_WhenWereRequested=recv_buffer.left(recv_buffer.indexOf("@"));
        recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);
        partnerHostPort_WhenWereRequested=recv_buffer.left(recv_buffer.indexOf("@")).toInt();

        emit weWereRequested();
        qDebug()<<"We were requested by(" + partnerHostIP_WhenWereRequested + \
                  " : " <<partnerHostPort_WhenWereRequested<<\
                  ")! { From:TcpConnecterWithServer::deal_con_ret(QString recv_buffer_orig) }";

        //then keep this connection alive
        sendKeepAliveRequest();
        break;
    case REQUEST_UNKOWN_ERROR:
        break;
    case RECEIVED_STRING_TOO_SHORT:
        break;
    default:
        break;
    }
}
void TcpConnecterWithServer::replyServer_WeHaveSendMessageToPartner(QByteArray *send_buffer_byte)
{
    if(!conTCP)
    {
        QMessageBox::critical(0, QObject::tr("TCP连接错误："),QObject::tr("请求伙伴信息时的TCP对象已被销毁！"));
        return;
    }
    conTCP->write(*send_buffer_byte);
    send_buffer_byte->~QByteArray();
}
void TcpConnecterWithServer::sendDisconnectRequest()
{
    if(!conTCP)
    {
        QMessageBox::critical(0, QObject::tr("TCP连接错误："),QObject::tr("请求伙伴信息时的TCP对象已被销毁！"));
        return;
    }
    QByteArray send_buffer_byte;
    send_buffer_byte.resize(INT_BYTE_LENGTH);
    MY_MACRO_INT nut=DISCONNECT;
    memcpy(send_buffer_byte.data(),&nut,INT_BYTE_LENGTH);
    QString send_buffer;
    send_buffer.append(userId + "@" + userPasswd + "@");
    int n = localHostInfo_s->numOfLocalHostMac();
    int j = 0;
    for(j=0;j<n;j++)
    {
        send_buffer.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
        send_buffer.append("@");
    }
    send_buffer_byte += send_buffer.toLatin1();
    conTCP->write(send_buffer_byte);
}
void TcpConnecterWithServer::sendKeepAliveRequest()
{
    QByteArray send_buffer_byte;
    send_buffer_byte.resize(INT_BYTE_LENGTH);
    MY_MACRO_INT nut=KEEP_ALIVE_SIGNAL;
    memcpy(send_buffer_byte.data(),&nut,INT_BYTE_LENGTH);
    QString send_buffer;
    int n=0,j=0;

    //then keep this connection alive
    send_buffer.append(userId + "@");
    n = localHostInfo_s->numOfLocalHostMac();
    for(j=0;j<n;j++)
    {
        send_buffer.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
        send_buffer.append("@");
    }
    qDebug()<<send_buffer + Tool::timeSign()<<"{ From:TcpConnecterWithServer::sendKeepAliveRequest() }";

    send_buffer_byte += send_buffer.toLatin1();
    conTCP->write(send_buffer_byte);
}
void TcpConnecterWithServer::afterSendShareScreenRequests(QString recv_buffer_orig)
{
    QString recv_buffer(recv_buffer_orig);
    //recv_buffer=recv_buffer.mid(recv_buffer.indexOf("@") + 1);

    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case PARTNER_HOST_ARE_ALSO_READY_TO_BE_CONNECTED:
        QMessageBox::critical(0, QObject::tr("请求连接结果"),QObject::tr("对方接受了您的连接请求！"));
        emit preparTowShowScreenPicture();
        break;
    case YOUR_PARTNER_DONOT_ALLOW_TO_BE_CONNECTED:
        QMessageBox::critical(0, QObject::tr("请求连接结果"),QObject::tr("对方拒绝了您的连接请求！"));
        break;
    default:
        break;
    }
}
void TcpConnecterWithServer::weHaveReceivedScreenData(QByteArray recv_buffer_orig)
{
    //QByteArray recv_byte_buffer(recv_buffer_orig.mid(recv_buffer_orig.indexOf("@") + 1).toUtf8());
    emit showScreenShotReceived(recv_buffer_orig);
}
void TcpConnecterWithServer::partnerWillExitAPP(QString recv_buffer_orig)
{
    qDebug()<<"Your partner have exit this APP!\t"<<recv_buffer_orig;
    emit yourPartnerHaveExit();
    QMessageBox::critical(0, QObject::tr("伙伴已下线："),QObject::tr("当前连接将断开！"));
}
void TcpConnecterWithServer::sendScreenData(QByteArray *send_buffer)
{
    if(!conTCP)
    {
        QMessageBox::critical(0, QObject::tr("TCP连接错误："),QObject::tr("请求伙伴信息时的TCP对象已被销毁！"));
        return;
    }
    int send_length = conTCP->write(*send_buffer);
    qDebug()<<"For sending this screenshot, we have sent  "<<send_length<<"bytes.";

    send_buffer->~QByteArray();
}
void TcpConnecterWithServer::requestedHostNotExists()
{
    QMessageBox::critical(0, QObject::tr("主机请求错误："),QObject::tr("对不起，您请求的伙伴不存在！"));
}
void TcpConnecterWithServer::requestedYourSelf()
{
    QMessageBox::critical(0, QObject::tr("主机请求错误："),QObject::tr("对不起，您不能请求自己哦！"));
}
