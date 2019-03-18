#ifndef LOCALHOSTINFO_H
#define LOCALHOSTINFO_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include <QNetworkInterface>

class LocalHostInfo
{
public:
    LocalHostInfo();
    void appendLocalHostIpV4(QHostAddress *ipv4);
    void appendLocalHostMac(QNetworkInterface *mac);
    QHostAddress *getLocalHostIpV4_At(int index);
    QNetworkInterface *getLocalHostMac_At(int index);
    int numOfLocalHostIpV4();
    int numOfLocalHostMac();

private:
    QList<QHostAddress*> localHostIpV4;
    QList<QNetworkInterface*> localHostMac;
};

#endif // LOCALHOSTINFO_H
