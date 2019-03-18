#include "connectByPartnerHostAndShareScreen.h"

ConnectByPartnerHostAndShareScreen::ConnectByPartnerHostAndShareScreen(QObject *parent) : QObject(parent)
{
    currentThread=NULL;
    conUDP=NULL;
    mMedia=NULL;
}
void ConnectByPartnerHostAndShareScreen::doWork(QThread *self)
{
    currentThread=self;

    stopFlag_ConnectPartnerHostAndGetScreen_Threader=true;

    listenUDPPortAndWaitMessage();
}
int ConnectByPartnerHostAndShareScreen::listenUDPPortAndWaitMessage()
{
    /*
    if(conUDP)
    {
        conUDP->close();
        conUDP->~QUdpSocket();
    }
    conUDP=new QUdpSocket();
    if(!conUDP->bind(userLocalPort,QAbstractSocket::ReuseAddressHint|QAbstractSocket::ShareAddress))
    {
        //use share mode bind port
        qDebug()<<"Bind port faild please retry! { From:ConnectByPartnerHostAndShareScreen::listenUDPPortAndWaitMessage() }" + Tool::timeSign();
        return -1;
    }
    connect(conUDP, SIGNAL(readyRead()), this, SLOT(readUDPData()));
    connect(conUDP, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readError(QAbstractSocket::SocketError)));
    //conUDP->connectToHost(QHostAddress(partnerHostIP),partnerHostPort);
    */

    connectPartnerForwardly_WhenWeWererequested();//synchronization

    return 0;
}
void ConnectByPartnerHostAndShareScreen::connectPartnerForwardly_WhenWeWererequested()
{//synchronization
    /*
    if(!conUDP)
    {
        qDebug()<<"Connect partner forwardly faild please retry! { From:ConnectByPartnerHostAndShareScreen::connectPartnerForwardly_WhenWeWererequested() }" + Tool::timeSign();
        return;
    }

    QString send_buffer(QString::number(I_AM_READY_TO_BE_CONNECTED) + "@" + QString::number(1));

    QMessageBox::critical(0, QObject::tr("About Connect"),QObject::tr("Message had sended"));
    QByteArray datagram(send_buffer.toUtf8());

    //////////////////////////////////////////////////////
    int tt=5;
    while(tt--)
    {
        currentThread->msleep(100);
        conUDP->writeDatagram(datagram,QHostAddress(partnerHostIP),partnerHostPort);
    }
    //////////////////////////////////////////////////////

    conUDP->writeDatagram(datagram,QHostAddress(partnerHostIP_WhenWereRequested),partnerHostPort_WhenWereRequested);
    //conUDP->write(send_buffer.toLatin1());
    */

    QString reply_string;
    QByteArray *send_buffer = NULL;
    MY_MACRO_INT nut = 0;

    QMessageBox::StandardButton rb = QMessageBox::question(NULL, "伙伴请求共享您的屏幕", "是否允许？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        int n = localHostInfo_s->numOfLocalHostMac();
        int j = 0;
        for(j=0;j<n;j++)
        {
            reply_string.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
            reply_string.append("@");
        }

        send_buffer = new QByteArray();
        send_buffer->resize(INT_BYTE_LENGTH + reply_string.length() + 1);
        nut=I_AM_READY_TO_BE_CONNECTED;
        memcpy(send_buffer->data(), &nut, INT_BYTE_LENGTH);
        memcpy(send_buffer->data() + INT_BYTE_LENGTH, reply_string.toLatin1().data(), reply_string.length());

        emit replyServer_WeHaveSendMessageToPartner(send_buffer);
        emit preparOurScreenPicture();
        send_buffer = NULL;

        getOurScreenPicture();//screenshot

    }else
    {
        int n = localHostInfo_s->numOfLocalHostMac();
        int j = 0;
        for(j=0;j<n;j++)
        {
            reply_string.append(localHostInfo_s->getLocalHostMac_At(j)->hardwareAddress());
            reply_string.append("@");
        }

        send_buffer = new QByteArray();
        send_buffer->resize(INT_BYTE_LENGTH + reply_string.length() + 1);
        nut=I_AM_NOT_ALLOW_TO_BE_CONNECTED;
        memcpy(send_buffer->data(), &nut, INT_BYTE_LENGTH);
        memcpy(send_buffer->data() + INT_BYTE_LENGTH, reply_string.toLatin1().data(), reply_string.length());

        emit replyServer_WeHaveSendMessageToPartner(send_buffer);
        send_buffer = NULL;
    }
}
void ConnectByPartnerHostAndShareScreen::readUDPData()
{
    QMessageBox::critical(0, QObject::tr("About UDP Connect"),QObject::tr("Message had received"));
    while(conUDP->hasPendingDatagrams())
    {
        QByteArray te;
        te.resize(conUDP->pendingDatagramSize());
        conUDP->readDatagram(te.data(),te.size());
        QString recv_buffer(te.data());

        //QString recv_buffer(conUDP->readAll());
        qDebug()<<recv_buffer + Tool::timeSign()<<" { From:ConnectByPartnerHostAndShareScreen::readUDPData() }";
        switch(recv_buffer.left(recv_buffer.indexOf("@")).toInt())
        {
        case REQUEST_TO_SHARE_SCREEN:
            qDebug()<<"REQUEST_TO_SHARE_SCREEN { From:ConnectByPartnerHostAndShareScreen::readUDPData() }" + Tool::timeSign();
            break;
        case REQUEST_TO_CONTROL_DEVICE_BY_REMOTE:
            break;
        default:
            break;
        }
    }
}

void ConnectByPartnerHostAndShareScreen::readError(QAbstractSocket::SocketError)
{
    ;
}
void ConnectByPartnerHostAndShareScreen::getOurScreenPicture()
{
    if(mMedia)
        mMedia->~MyMedia();
    mMedia = new MyMedia();

    QByteArray *send_buffer = NULL, *pic_buffer = NULL;

    MY_MACRO_INT const fl = THIS_IS_PICTURE_DATA;
    MY_MACRO_INT length_of_data = 0;

    while(stopFlag_ConnectPartnerHostAndGetScreen_Threader)
    {
        send_buffer=new QByteArray();
        pic_buffer = mMedia->getNextScreenShot();

        length_of_data = pic_buffer->size();

        send_buffer->resize(INT_BYTE_LENGTH * 2 + length_of_data);
        memcpy(send_buffer->data(),&fl,INT_BYTE_LENGTH);
        memcpy(send_buffer->data() + INT_BYTE_LENGTH, &length_of_data, INT_BYTE_LENGTH);
        memcpy(send_buffer->data() + INT_BYTE_LENGTH * 2, pic_buffer->data(), length_of_data);

        emit sendScreenData(send_buffer);
        qDebug()<<"A screenshot picture has been shot, and it's length is "<<length_of_data<<".";

        pic_buffer->~QByteArray();
        pic_buffer = NULL;
        send_buffer = NULL;

        length_of_data = 0;

        currentThread->msleep(100);
    }
}
