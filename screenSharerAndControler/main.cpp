#include "mainwindow.h"
#include "variable.h"
#include "tool.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

//This APP's full name is screenSharerAndControler.
//And it's brief name is SSAC.

int ratio;
char * dataEnd="TheEnd";

#ifdef DESKTOP
QString picRootPath("../screenSharerAndControler/image/");
QString resourceRootPath("../screenSharerAndControler/resource/");
QString appSdcardPath("../screenSharerAndControler/");
#endif
#ifdef ANDROID
QString picRootPath("assets:/resource/image/");
QString resourceRootPath("assets:/resource/");
QString appSdcardPath("/sdcard/SSAC/");
#endif
//QString databaseName("idiom.db");
//QString databasePath("resource/");
QString logPath("log/");
QString logName("SSAC.log");
QString screenshotPath("screenshot/");
QString screenshot_PrefixName("screen");
QString screenshot_SuffixName(".jpg");
QSize *screenSize;
//QSize *editBoxSize;
QSize *windowSize;

QString serverIP("192.168.1.101");
int serverDefaultPort=48690;//服务器默认准备监听的端口
int port_max_difference_value=5;//如果端口被占用，则尝试进行另一个端口的最大差值
int serverFinalPort=serverDefaultPort;//程序最终绑定使用的端口值

QString userId;
QString userPasswd;
QString selfIP;
int selfPort;
QString partnerHostID;
QString partnerHostPasswd;
QString partnerHostIP;
int partnerHostPort;
QString partnerHostIP_WhenWereRequested;
int partnerHostPort_WhenWereRequested;
int userLocalPort=50509;

LocalHostInfo *localHostInfo_s;

bool stopFlag_ConnectPartnerHostAndGetScreen_Threader = false;

void init();
void customMessageHandler(QtMsgType type, const QMessageLogContext& Context, const QString &sMsg);

int main(int argc, char *argv[])
{
#ifdef DESKTOP
    QApplication::addLibraryPath("./plugins");
#endif
    QApplication a(argc, argv);
    init();
    MainWindow w;
    w.show();

    return a.exec();
}
void init()
{
    localHostInfo_s=new LocalHostInfo();
    QDir dir;

#ifdef ANDROID
    QByteArray data;
    QFile oriDb(resourceRootPath);
    QFile finalDb(appSdcardPath);
    dir.mkpath(appSdcardPath);//Create multi-level directory
    dir.mkpath(appSdcardPath + logPath);
    dir.mkpath(appSdcardPath + screenshotPath);


    //first check whether the database file exist,if no,create folder and copy it from "assest:/resource/"
    if(finalDb.exists() && QFileInfo(finalDb.fileName()).isFile() && finalDb.size())//if the file exist
    {
        goto neednotCreatFile;
    }

    if(finalDb.exists())
    {
        if(QFileInfo(finalDb.fileName()).isFile())
            finalDb.remove();
        else
            dir.rmdir(finalDb.fileName());
    }

    if(oriDb.open(QFile::ReadOnly))
    {
        data = oriDb.readAll();
        oriDb.close();
    }else
    {
        qDebug() << oriDb.fileName() << " file open failed " << oriDb.errorString();
    }

    if(finalDb.open(QFile::WriteOnly))
    {
        finalDb.write(data);
        finalDb.close();
    }else
    {
        qDebug() << finalDb.fileName() << " file open failed" << finalDb.errorString();
    }

    neednotCreatFile:

#endif

    QFile logFile(appSdcardPath + logPath + logName);
    if(logFile.exists() && QFileInfo(logFile.fileName()).isFile() && logFile.size())
        logFile.remove();
    else if(logFile.exists())
        {
            if(QFileInfo(logFile.fileName()).isDir())
                dir.rmdir(logFile.fileName());
        }

#ifndef DEBUG
    qInstallMessageHandler(customMessageHandler);//注册日志处理回调函数
#endif
    qDebug()<<"当前的图片资源路径是 "<<picRootPath<<"\t现在是void init()";
    qDebug()<<"当前的数据资源路径是 "<<resourceRootPath<<"\t现在是void init()";

    int width=QApplication::desktop()->width();
    int height=QApplication::desktop()->height();
    ratio=QApplication::desktop()->screen()->devicePixelRatio();
#ifdef ANDROID
    windowSize=new QSize(width,height);
#endif
#ifdef DESKTOP
    windowSize=new QSize(Tool::LogicalScreenSize.width(),(double)Tool::LogicalScreenSize.height());
#endif
    screenSize=new QSize(windowSize->width(),windowSize->height());//初始化屏幕尺寸记录变量
}
void customMessageHandler(QtMsgType type, const QMessageLogContext& Context, const QString &sMsg)
{
    QString txt;
    switch (type)
    {//调试信息提示
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(sMsg);
        break;
        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(sMsg);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(sMsg);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(sMsg);
        abort();
    }
    QFile logFile(appSdcardPath + logPath + logName);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&logFile);
    ts << "\r\n" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd") <<'\t' << txt << endl;
}
