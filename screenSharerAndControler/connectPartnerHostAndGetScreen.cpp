#include "connectPartnerHostAndGetScreen.h"
#include <QGuiApplication>

ConnectPartnerHostAndGetScreen::ConnectPartnerHostAndGetScreen(QObject *parent) : QObject(parent)
{
    currentThread=NULL;
    conUDP=NULL;
}
void ConnectPartnerHostAndGetScreen::doWork(QThread *self)
{
    currentThread=self;
    qDebug()<<"From :{ConnectPartnerHostAndGetScreen::doWork(QThread *self)}";

    //request Partner host information
    reqPartnerHostInfo();
}
int ConnectPartnerHostAndGetScreen::reqPartnerHostInfo()
{
    QByteArray send_buffer_byte;
    send_buffer_byte.resize(INT_BYTE_LENGTH);
    MY_MACRO_INT nut=REQUEST_PARTNER_HOST_INFO;
    memcpy(send_buffer_byte.data(),&nut,INT_BYTE_LENGTH);

    QString send_buffer;
    send_buffer.append(partnerHostID + "@" + partnerHostPasswd + "@");
    int n = localHostInfo_s->numOfLocalHostMac();
    int j = 0;
    for(j=0;j<n;j++)
    {
        send_buffer.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
        send_buffer.append("@");
    }
    send_buffer_byte += send_buffer.toLatin1();
    emit requestPartnerHostInfo(send_buffer_byte);
}
void ConnectPartnerHostAndGetScreen::readError(QAbstractSocket::SocketError socketE)
{
    ;
}
void ConnectPartnerHostAndGetScreen::startForwardly_ConnctPartnerHost()
{
    if(conUDP)
    {
        conUDP->close();
        conUDP->~QUdpSocket();
    }
    conUDP=new QUdpSocket();
    if(!conUDP->bind(userLocalPort,QAbstractSocket::ReuseAddressHint|QAbstractSocket::ShareAddress))
    {
        //use share mode bind port
        qDebug()<<"Bind port faild please retry! { From:ConnectPartnerHostAndGetScreen::connctPartnerHost() }" + Tool::timeSign();
        return;
    }
    connect(conUDP, SIGNAL(readyRead()), this, SLOT(readUDPData()));
    connect(conUDP, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readError(QAbstractSocket::SocketError)));
    /*conUDP->connectToHost(QHostAddress(partnerHostIP),partnerHostPort);

    int i=1;
    for(i=1;i<=CONNECT_TIMES;i++)
    {//connect for many times
        if (conUDP->waitForConnected(1000)) //success
        {
            break;
        }
        qDebug()<<"Connect to partner host faild for the "<<i<<"'s times. { From:ConnectPartnerHostAndGetScreen::connctPartnerHost() }";
    }
    if(i > CONNECT_TIMES)
    {
        qDebug()<<"Connect to partner host faild for all "<<CONNECT_TIMES<<" times,please retry! { From:ConnectPartnerHostAndGetScreen::connctPartnerHost() }";
        return -1;
    }*/

    QString send_buffer(QString::number(REQUEST_TO_SHARE_SCREEN));
    send_buffer.append("@");
    int n = localHostInfo_s->numOfLocalHostMac();
    int j = 0;
    for(j=0;j<n;j++)
    {
        send_buffer.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
        send_buffer.append("@");
    }
    QMessageBox::critical(0, QObject::tr("About Connect"),QObject::tr("Message had sended"));
    QByteArray datagram(send_buffer.toUtf8());

    //////////////////////////////////////////////////////
    int tt=5;
    while(tt--)
    {
        currentThread->sleep(1);
        conUDP->writeDatagram(datagram,QHostAddress(partnerHostIP),partnerHostPort);
    }
    //////////////////////////////////////////////////////

    conUDP->writeDatagram(datagram,QHostAddress(partnerHostIP),partnerHostPort);
    //conUDP->write(send_buffer.toLatin1());
}
void ConnectPartnerHostAndGetScreen::readUDPData()
{
    QMessageBox::critical(0, QObject::tr("About UDP Connect"),QObject::tr("Have received reply message."));

    QString recv_buffer(conUDP->readAll());

    qDebug()<<recv_buffer <<" { From:ConnectPartnerHostAndGetScreen::readUDPData() }";
    switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
    {
    case REQUEST_TO_SHARE_SCREEN:
        //deal_con_ret(recv_buffer);
        break;
    case REQUEST_TO_CONTROL_DEVICE_BY_REMOTE:
        break;
    default:
        break;
    }
}
