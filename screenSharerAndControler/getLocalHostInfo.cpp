#include "getLocalHostInfo.h"

GetLocalHostInfo::GetLocalHostInfo()
{

}
int GetLocalHostInfo::getLocalHostIpV4Address(LocalHostInfo *lhi)
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    int num = 0;
    for (int i = 0; i < nListSize; ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
        {
            QHostAddress *temp=new QHostAddress(ipAddressesList.at(i));
            lhi->appendLocalHostIpV4(temp);
            num++;
        }
    }
    // 如果没有找到，则以本地IP地址为IP
    if (!num)
    {
        QHostAddress *temp=new QHostAddress(QHostAddress::LocalHost);
        lhi->appendLocalHostIpV4(temp);
    }
    return num;
}
int GetLocalHostInfo::getLocalHostMacAddress(LocalHostInfo *lhi)
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    int num=0;
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QNetworkInterface *temp=new QNetworkInterface(nets[i]);
            lhi->appendLocalHostMac(temp);
            num++;
        }
    }
    return num;
}
