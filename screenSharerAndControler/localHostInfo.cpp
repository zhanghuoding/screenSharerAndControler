#include "localHostInfo.h"

LocalHostInfo::LocalHostInfo()
{

}
void LocalHostInfo::appendLocalHostIpV4(QHostAddress *ipv4)
{
    this->localHostIpV4.append(ipv4);
}
void LocalHostInfo::appendLocalHostMac(QNetworkInterface *mac)
{
    this->localHostMac.append(mac);
}
QHostAddress *LocalHostInfo::getLocalHostIpV4_At(int index)
{
    return this->localHostIpV4.at(index);
}
QNetworkInterface *LocalHostInfo::getLocalHostMac_At(int index)
{
    return this->localHostMac.at(index);
}
int LocalHostInfo::numOfLocalHostIpV4()
{
    return this->localHostIpV4.size();
}
int LocalHostInfo::numOfLocalHostMac()
{
    return this->localHostMac.size();
}
