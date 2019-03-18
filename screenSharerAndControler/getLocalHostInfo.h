#ifndef GETLOCALHOSTINFO_H
#define GETLOCALHOSTINFO_H

#include <QObject>
#include <QNetworkInterface>
#include <QList>
#include "localHostInfo.h"

class GetLocalHostInfo
{
public:
    GetLocalHostInfo();

    int getLocalHostIpV4Address(LocalHostInfo *lhi);
    int getLocalHostMacAddress(LocalHostInfo *lhi);
};

#endif // GETLOCALHOSTINFO_H
