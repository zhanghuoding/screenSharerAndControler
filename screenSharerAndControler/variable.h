#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QMessageBox>
#include "localHostInfo.h"

typedef QT_PREPEND_NAMESPACE(quintptr) WId;

#define MY_MACRO_INT quint32
#define INT_BYTE_LENGTH sizeof(MY_MACRO_INT)

#define CONNECT_TIMES 10

#define QUEUE 200
#define BUFFER_SIZE (1024*512)
#define MAX_THREAD_NUM 200
#define IP_SIZE 48
#define MAC_SIZE 64

#define CONNECT 1
#define REQUEST_PARTNER_HOST_INFO 2
#define DISCONNECT 3
#define KEEP_ALIVE_SIGNAL 44//This is for keepalive, but it is disabled now. If you want to enable it, set it to a value of 4.

#define REQUESTED_HOST_NOT_EXISTS 5
#define REQUESTED_YOUR_SELF 6
#define REQUEST_SUCCESS 7
#define REQUEST_UNKOWN_ERROR 8
#define RECEIVED_STRING_TOO_SHORT 9

#define REQUEST_TO_SHARE_SCREEN 10
#define REQUEST_TO_CONTROL_DEVICE_BY_REMOTE 11
#define YOU_WARE_REQUESTED_TO_BE_CONNECTED_BY_THIS_HOST 12
#define I_AM_READY_TO_BE_CONNECTED 13
#define PARTNER_HOST_ARE_ALSO_READY_TO_BE_CONNECTED 14
#define I_AM_NOT_ALLOW_TO_BE_CONNECTED 15
#define YOUR_PARTNER_DONOT_ALLOW_TO_BE_CONNECTED 16
#define I_WILL_EXIT 17

#define THIS_IS_PICTURE_DATA 18
#define CURRENT_DATA_IS_TOO_LONG 19

extern char * dataEnd;
#define END_FLAG_LENGTH strlen(dataEnd)

extern int ratio;
extern QString picRootPath;//图片资源的根目录
extern QString resourceRootPath;//资源文件的根目录
extern QString appSdcardPath;//Application's root folder
//extern QString databaseName;//Application's database name
//extern QString databasePath;//database's path
extern QString logPath;
extern QString logName;
extern QString screenshotPath;
extern QString screenshot_PrefixName;
extern QString screenshot_SuffixName;

extern QSize *screenSize;//设备的屏幕尺寸
extern QSize *windowSize;//窗口尺寸

extern QString serverIP;
extern int serverDefaultPort;//服务器默认准备监听的端口
extern int port_max_difference_value;//如果端口被占用，则尝试进行另一个端口的最大差值
extern int serverFinalPort;//程序最终绑定使用的端口值

extern QString userId;
extern QString userPasswd;
extern QString selfIP;
extern int selfPort;
extern QString partnerHostID;
extern QString partnerHostPasswd;
extern QString partnerHostIP;
extern int partnerHostPort;
extern QString partnerHostIP_WhenWereRequested;
extern int partnerHostPort_WhenWereRequested;
extern int userLocalPort;

extern LocalHostInfo *localHostInfo_s;

extern bool stopFlag_ConnectPartnerHostAndGetScreen_Threader;

#endif // VARIABLE_H
