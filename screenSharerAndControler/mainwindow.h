#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QKeyEvent>
#include <QWidget>
#include <QTime>
#include <QLineEdit>
#include <QThread>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include "rootwidget.h"
#include "variable.h"
#include "imageButton.h"
#include "pQWidget.h"
#include "tool.h"
#include "newLineEdit.h"
#include "tcpConnecterWithServer.h"
#include "connectPartnerHostAndGetScreen.h"
#include "connectByPartnerHostAndShareScreen.h"
#include "controlRemoteDevice.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QByteArray *picDataBuffer;
    int picDataLength;
    bool isNewPicData;

    RootWidget *rootWidget;
    PQWidget *convertionalWidget;
    PQWidget *screenSharedVideoWidget;
    QPushButton *partnerScreenPictureShower;
    PQWidget *settingWidget;
    PQWidget *helpWidget;

    PQWidget *centWidget;

    PQWidget *connectPageWidget;
    PQWidget *computerPageWidget;
    PQWidget *chatPageWidget;
    PQWidget *topBar;
    PQWidget *bottomBar;
    ImageButton *chatButton;
    ImageButton *computerButton;
    ImageButton *connectButton;
    ImageButton *menuButton;

    QWidget *menuWidget;
    ImageButton *settingButtonInMenu;
    ImageButton *helpButtonInMenu;

    QLabel *userIDShower;
    QLabel *userPasswdShower;
    NewLineEdit *partnerIdInputBox;
    NewLineEdit *partnerPasswdInputBox;

    ImageButton *getSharedScreenButton;
    ImageButton *controlRemoteDeviceButton;
//=======================================================
    ImageButton *testButton;
//=======================================================

    QThread tcpConnecterWithServer_Threader;
    TcpConnecterWithServer *tcpConnecterWithServer;
    QThread connectPartnerHostAndGetScreen_Threader;
    ConnectPartnerHostAndGetScreen *connectPartnerHostAndGetScreen;
    QThread connectByPartnerHostAndShareScreen_Threader;
    ConnectByPartnerHostAndShareScreen *connectByPartnerHostAndShareScreen;
    QThread controlRemoteDevice_Threader;
    ControlRemoteDevice *controlRemoteDevice;

    NewLineEdit *serverIPInputBox;
    NewLineEdit *serverPortInputBox;


    void initConvertionalWidget();
    void initScreenSharedVideoWidget();
    void initSettingWidget();
    void initHelpWidget();

    void initConnectPageWidget();
    void initComputerPageWidget();
    void initChatPageWidget();
#ifdef ANDROID
    QTime time;
#endif
protected:
#ifdef ANDROID
    void keyPressEvent(QKeyEvent * event)Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent * event)Q_DECL_OVERRIDE;
#endif
signals:
    void startDoSomeInitializeWork(QThread *);
    void startConnectPartnerHost(QThread *);
    void startConnectPartner_WhenWeWereRequested(QThread *);
    void disConnectWithServer();
public slots:
    void changeMainPage(int flag,bool isClicked=true);
    void networkError(QString err,int no=REQUEST_UNKOWN_ERROR);
    void userID_Passwd_changed(QString iD,QString passwd);
    void weWereRequested();
    void exitApplication(int flag);

    void showScreenShotReceived(QByteArray recv_byte_buffer);
    void yourPartnerHaveExit();

    void serverFinalPortChanged();
};


#endif // MAINWINDOW_H
