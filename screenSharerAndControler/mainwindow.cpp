#include "mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef ANDROID
    time.start();
#endif

    picDataBuffer=NULL;
    picDataLength=0;
    isNewPicData=true;

    //do some initialize work,for example:register host,get ID and passwd
    tcpConnecterWithServer=new TcpConnecterWithServer();
    connect(this,SIGNAL(startDoSomeInitializeWork(QThread*)),tcpConnecterWithServer,SLOT(doWork(QThread*)));
    connect(tcpConnecterWithServer,SIGNAL(networkError(QString,int)),this,SLOT(networkError(QString,int)));
    connect(tcpConnecterWithServer,SIGNAL(userID_Passwd_Changed(QString,QString)),this,SLOT(userID_Passwd_changed(QString,QString)));
    connect(tcpConnecterWithServer,SIGNAL(weWereRequested()),this,SLOT(weWereRequested()));
    connect(this,SIGNAL(disConnectWithServer()),tcpConnecterWithServer,SLOT(sendDisconnectRequest()));
    connect(tcpConnecterWithServer,SIGNAL(OK_exit(int)),this,SLOT(exitApplication(int)));
    connect(tcpConnecterWithServer,SIGNAL(showScreenShotReceived(QByteArray)),this,SLOT(showScreenShotReceived(QByteArray)));
    connect(tcpConnecterWithServer,SIGNAL(yourPartnerHaveExit()),this,SLOT(yourPartnerHaveExit()));
    connect(tcpConnecterWithServer,SIGNAL(serverFinalPortChanged()),this,SLOT(serverFinalPortChanged()));
    tcpConnecterWithServer->moveToThread(&tcpConnecterWithServer_Threader);
    tcpConnecterWithServer_Threader.start();
    emit startDoSomeInitializeWork(&tcpConnecterWithServer_Threader);

    //Request Partner host's ip and port from server,and then connect with it.
    connectPartnerHostAndGetScreen=new ConnectPartnerHostAndGetScreen();
    connect(this,SIGNAL(startConnectPartnerHost(QThread*)),connectPartnerHostAndGetScreen,SLOT(doWork(QThread*)));
    connect(connectPartnerHostAndGetScreen,SIGNAL(networkError(QString,int)),this,SLOT(networkError(QString,int)));
    connect(connectPartnerHostAndGetScreen,SIGNAL(requestPartnerHostInfo(QByteArray)),tcpConnecterWithServer,SLOT(requestPartnerHostInfo(QByteArray)));
    connect(tcpConnecterWithServer,SIGNAL(startForwardly_ConnctPartnerHost()),connectPartnerHostAndGetScreen,SLOT(startForwardly_ConnctPartnerHost()));
    connectPartnerHostAndGetScreen->moveToThread(&connectPartnerHostAndGetScreen_Threader);
    connectPartnerHostAndGetScreen_Threader.start();

    //Listen local port and deal with partner connect request.
    connectByPartnerHostAndShareScreen=new ConnectByPartnerHostAndShareScreen();
    connect(this,SIGNAL(startConnectPartner_WhenWeWereRequested(QThread*)),connectByPartnerHostAndShareScreen,SLOT(doWork(QThread*)));
    connect(connectByPartnerHostAndShareScreen,SIGNAL(networkError(QString,int)),this,SLOT(networkError(QString,int)));
    connect(connectByPartnerHostAndShareScreen,SIGNAL(replyServer_WeHaveSendMessageToPartner(QByteArray*)),tcpConnecterWithServer,SLOT(replyServer_WeHaveSendMessageToPartner(QByteArray*)));
    connect(connectByPartnerHostAndShareScreen,SIGNAL(sendScreenData(QByteArray*)),tcpConnecterWithServer,SLOT(sendScreenData(QByteArray*)));
    connectByPartnerHostAndShareScreen->moveToThread(&connectByPartnerHostAndShareScreen_Threader);
    connectByPartnerHostAndShareScreen_Threader.start();

    controlRemoteDevice=new ControlRemoteDevice();

    this->setMinimumSize(windowSize->width(),windowSize->height());
    this->setMaximumSize(windowSize->width(),windowSize->height());
    this->resize(windowSize->width(),windowSize->height());
    //this->setCentralWidget(rootWidget);
    //this->setStyleSheet("background-color:#c62f2f;");

    rootWidget=new RootWidget(this);
    rootWidget->setGeometry(rootWidget->parentWidget()->rect());
    rootWidget->setMinimumSize(rootWidget->parentWidget()->minimumSize());
    rootWidget->setMaximumSize(rootWidget->parentWidget()->maximumSize());
    rootWidget->resize(rootWidget->parentWidget()->size());
    //rootWidget->setStyleSheet("background-color:#c62f2f;");

    initScreenSharedVideoWidget();
    initConvertionalWidget();
    initSettingWidget();
    initHelpWidget();
}

MainWindow::~MainWindow()
{

}

#ifdef ANDROID
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //if the "Back" key on android was pressed two consecutive times in 2 seconds,
    //then,the application will exit.
    if(event->key() == Qt::Key_Back)
        ;//do nothing
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Back)
    {
        //when the "Back" key on android was released
        //record the time or exit application(when pressed toe consecutive times in 2 seconds
        if(time.elapsed()/1000.0 > 2)//if time interval more than 2 seconds,restart the timer
            time.restart();
        else//if time interval less or equal to 2 seconds,then exit the application
            exitApplication(0);
    }
}
#endif

void MainWindow::changeMainPage(int flag, bool isClicked)
{
    qDebug()<<"Key "<< flag <<"was clicked!";

    menuWidget->setVisible(false);

    switch(flag)
    {
    case Tool::ButClass_ConnectButton:
        connectButton->setStyleSheet(Tool::retStyleStr("#000000","#333333","#000000"));
        computerButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        chatButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        connectPageWidget->setVisible(true);
        computerPageWidget->setVisible(false);
        chatPageWidget->setVisible(false);
        break;
    case Tool::ButClass_ComputerButton:
        connectButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        computerButton->setStyleSheet(Tool::retStyleStr("#000000","#333333","#000000"));
        chatButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        connectPageWidget->setVisible(false);
        computerPageWidget->setVisible(true);
        chatPageWidget->setVisible(false);
        break;
    case Tool::ButClass_ChatButton:
        connectButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        computerButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
        chatButton->setStyleSheet(Tool::retStyleStr("#000000","#333333","#000000"));
        connectPageWidget->setVisible(false);
        computerPageWidget->setVisible(false);
        chatPageWidget->setVisible(true);
        break;
    case Tool::ButClass_MenuButton:
        menuWidget->setParent(convertionalWidget);
        menuWidget->setVisible(true);
        break;
    case Tool::ButClass_SettingButton:
        initSettingWidget();
        convertionalWidget->setVisible(false);
        screenSharedVideoWidget->setVisible(false);
        settingWidget->setParent(rootWidget);
        settingWidget->setVisible(true);
        helpWidget->setVisible(false);
        break;
    case Tool::ButClass_HelpButton:
        //initHelpWidget();
        convertionalWidget->setVisible(false);
        screenSharedVideoWidget->setVisible(false);
        settingWidget->setVisible(false);
        helpWidget->setParent(rootWidget);
        helpWidget->setVisible(true);
        break;
    case Tool::ButClass_GetSharedScreenButton:
        //get Partner host's ID and passwd
        partnerHostID=partnerIdInputBox->text();
        partnerHostPasswd=partnerPasswdInputBox->text();
        //connect Partner host
        emit startConnectPartnerHost(&connectPartnerHostAndGetScreen_Threader);
        break;
    case Tool::ButClass_ControlRemoteDeviceButton:
        break;
    case Tool::ButClass_BackFromSetting:
        convertionalWidget->setParent(rootWidget);
        convertionalWidget->setVisible(true);
        screenSharedVideoWidget->setVisible(false);
        settingWidget->setVisible(false);
        helpWidget->setVisible(false);
        break;
    case Tool::ButClass_BackFromHelp:
        convertionalWidget->setParent(rootWidget);
        convertionalWidget->setVisible(true);
        screenSharedVideoWidget->setVisible(false);
        settingWidget->setVisible(false);
        helpWidget->setVisible(false);
        break;
    case Tool::ButClass_PartnerIdInputBox:
        break;
    case Tool::ButClass_PartnerPasswdInputBox:
        break;
    case Tool::ButClass_SettingPageEnterButton:
        serverIP = serverIPInputBox->text();
        serverDefaultPort = serverPortInputBox->text().toInt();
        emit startDoSomeInitializeWork(&tcpConnecterWithServer_Threader);

        changeMainPage(Tool::ButClass_BackFromSetting, true);
        break;
    case Tool::ButClass_ServerIPInputBox:
        break;
    case Tool::ButClass_ServerPortInputBox:
        break;
    case Tool::ButClass_TestButton:
//============================================================
//=============================================================
//==============================================================
        weWereRequested();
//==============================================================
//==============================================================
//==============================================================
        break;
    default:
        break;
    }
}
void MainWindow::initConvertionalWidget()
{

    convertionalWidget=new PQWidget("",Tool::ButClass_ConnectPageWidget,rootWidget);
    convertionalWidget->setGeometry(convertionalWidget->parentWidget()->rect());
    convertionalWidget->setMinimumSize(convertionalWidget->parentWidget()->minimumSize());
    convertionalWidget->setMaximumSize(convertionalWidget->parentWidget()->maximumSize());
    convertionalWidget->resize(convertionalWidget->parentWidget()->size());

    topBar=new PQWidget("",Tool::ButClass_TopBar,convertionalWidget);
    topBar->setGeometry(Tool::absRect(0,0,360,60));
    topBar->setStyleSheet("QWidget{background-color:#c62f2f;}");
    connect(topBar,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    centWidget=new PQWidget("",Tool::ButClass_CentWidget,convertionalWidget);
    centWidget->setGeometry(Tool::absRect(0,60,360,500));
    centWidget->setStyleSheet("background-color:#27de68;");
    connect(centWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    bottomBar=new PQWidget("",Tool::ButClass_BottomBar,convertionalWidget);
    bottomBar->setGeometry(Tool::absRect(0,560,360,60));
    bottomBar->setStyleSheet("background-color:#c62f2f;");
    connect(bottomBar,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    initComputerPageWidget();
    initChatPageWidget();
    initConnectPageWidget();

    menuButton=new ImageButton("",Tool::ButClass_MenuButton,topBar);
    menuButton->setText("菜单");
    menuButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    menuButton->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    menuButton->setGeometry(Tool::relRect(300,10,40,40));
    connect(menuButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    menuWidget=new QWidget();
    menuWidget->setGeometry(Tool::absRect(360-180-4,4,180,140));
    menuWidget->setStyleSheet("background-color:#f4f4f4;border-radius:6px");

    settingButtonInMenu=new ImageButton("",Tool::ButClass_SettingButton,menuWidget);
    settingButtonInMenu->setText("设置");
    settingButtonInMenu->setStyleSheet(Tool::retStyleStr("#c62f2f","#777777","#000000"));
    settingButtonInMenu->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    settingButtonInMenu->setGeometry(Tool::relRect(20,15,140,40));
    connect(settingButtonInMenu,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    helpButtonInMenu=new ImageButton("",Tool::ButClass_HelpButton,menuWidget);
    helpButtonInMenu->setText("帮助");
    helpButtonInMenu->setStyleSheet(Tool::retStyleStr("#c62f2f","#777777","#000000"));
    helpButtonInMenu->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    helpButtonInMenu->setGeometry(Tool::relRect(20,85,140,40));
    connect(helpButtonInMenu,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));
}
void MainWindow::initScreenSharedVideoWidget()
{
    screenSharedVideoWidget=new PQWidget("",Tool::ButClass_ScreenSharedVideoWidget,rootWidget);
    screenSharedVideoWidget->setGeometry(screenSharedVideoWidget->parentWidget()->rect());
    screenSharedVideoWidget->setMinimumSize(screenSharedVideoWidget->parentWidget()->minimumSize());
    screenSharedVideoWidget->setMaximumSize(screenSharedVideoWidget->parentWidget()->maximumSize());
    screenSharedVideoWidget->resize(screenSharedVideoWidget->parentWidget()->size());

    partnerScreenPictureShower=new QPushButton(screenSharedVideoWidget);
    screenSharedVideoWidget->setVisible(false);
}
void MainWindow::initSettingWidget()
{
    settingWidget=new PQWidget("",Tool::ButClass_SettingWidget,rootWidget);
    settingWidget->setGeometry(settingWidget->parentWidget()->rect());
    settingWidget->setMinimumSize(settingWidget->parentWidget()->minimumSize());
    settingWidget->setMaximumSize(settingWidget->parentWidget()->maximumSize());
    settingWidget->resize(settingWidget->parentWidget()->size());
    settingWidget->setParent(nullptr);

    QWidget *settingPageTopBar=new QWidget(settingWidget);
    settingPageTopBar->setGeometry(Tool::absRect(0,0,360,60));
    settingPageTopBar->setStyleSheet("QWidget{background-color:#c62f2f;}");


    ImageButton *backFromSetting=new ImageButton("",Tool::ButClass_BackFromSetting,settingPageTopBar);
    backFromSetting->setText("<< 返回");
    backFromSetting->setAlignment(Qt::AlignCenter);
    backFromSetting->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    backFromSetting->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    backFromSetting->setGeometry(Tool::relRect(10,10,80,40));
    connect(backFromSetting,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QLabel *conLabel_0=new QLabel(settingPageTopBar);
    conLabel_0->setText("设置");
    conLabel_0->setStyleSheet("QLabel{background-color:transparent;color:#ffffff;}");
    conLabel_0->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    conLabel_0->setGeometry(Tool::relRect(105,10,40,40));

    QWidget *conWidget_0=new QWidget(settingWidget);
    conWidget_0->setGeometry(Tool::absRect(0,60,360,560));
    conWidget_0->setStyleSheet("QWidget{background-color:transparent;}");

    ImageButton *enterButton=new ImageButton("",Tool::ButClass_SettingPageEnterButton,settingPageTopBar);
    enterButton->setText("确定");
    enterButton->setAlignment(Qt::AlignCenter);
    enterButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    enterButton->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    enterButton->setGeometry(Tool::relRect(270,10,80,40));
    connect(enterButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QWidget *serverIP_settingWidget=new QWidget(conWidget_0);
    serverIP_settingWidget->setGeometry(Tool::relRect(0,0,360,60));
    serverIP_settingWidget->setStyleSheet("QWidget{background-color:transparent;}");
    QLabel *conLabel_1=new QLabel(serverIP_settingWidget);
    conLabel_1->setText("服务器IP：");
    conLabel_1->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_1->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    conLabel_1->setGeometry(Tool::relRect(10,10,140,40));
    serverIPInputBox=new NewLineEdit(Tool::ButClass_ServerIPInputBox,serverIP_settingWidget);
    serverIPInputBox->setStyleSheet("QLineEdit{ background-color:transparent;border-width:0;\
border-style:outset;}QLineEdit:focus{color:#c62f2f}");
    serverIPInputBox->setGeometry(Tool::relRect(180,10,180,40));
    serverIPInputBox->setAlignment(Qt::AlignLeft);
    serverIPInputBox->setText(serverIP);
    serverIPInputBox->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::Bold));
    //connect(serverIPInputBox,SIGNAL(inFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    connect(serverIPInputBox,SIGNAL(outFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    QWidget *serverPort_settingWidget=new QWidget(conWidget_0);
    serverPort_settingWidget->setGeometry(Tool::relRect(0,60,360,60));
    serverPort_settingWidget->setStyleSheet("QWidget{background-color:#e4e4e4;}");
    QLabel *conLabel_2=new QLabel(serverPort_settingWidget);
    conLabel_2->setText("服务器端口：");
    conLabel_2->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_2->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    conLabel_2->setGeometry(Tool::relRect(10,10,140,40));
    serverPortInputBox=new NewLineEdit(Tool::ButClass_ServerPortInputBox,serverPort_settingWidget);
    serverPortInputBox->setStyleSheet("QLineEdit{ background-color:transparent;border-width:0;\
border-style:outset;}QLineEdit:focus{color:#c62f2f}");
    serverPortInputBox->setGeometry(Tool::relRect(180,10,180,40));
    serverPortInputBox->setAlignment(Qt::AlignLeft);
    serverPortInputBox->setText(QString::number(serverFinalPort));
    serverPortInputBox->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::Bold));
    //connect(serverPortInputBox,SIGNAL(inFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    connect(serverPortInputBox,SIGNAL(outFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
}
void MainWindow::initHelpWidget()
{
    helpWidget=new PQWidget("",Tool::ButClass_HelpWidget,rootWidget);
    helpWidget->setGeometry(helpWidget->parentWidget()->rect());
    helpWidget->setMinimumSize(helpWidget->parentWidget()->minimumSize());
    helpWidget->setMaximumSize(helpWidget->parentWidget()->maximumSize());
    helpWidget->resize(helpWidget->parentWidget()->size());
    helpWidget->setParent(nullptr);

    QWidget *helpPageTopBar=new QWidget(helpWidget);
    helpPageTopBar->setGeometry(Tool::absRect(0,0,360,60));
    helpPageTopBar->setStyleSheet("QWidget{background-color:#c62f2f;}");


    ImageButton *backFromHelp=new ImageButton("",Tool::ButClass_BackFromHelp,helpPageTopBar);
    backFromHelp->setText("<< 返回");
    backFromHelp->setAlignment(Qt::AlignCenter);
    backFromHelp->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    backFromHelp->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    backFromHelp->setGeometry(Tool::relRect(10,10,80,40));
    connect(backFromHelp,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QLabel *conLabel_0=new QLabel(helpPageTopBar);
    conLabel_0->setText("帮助");
    conLabel_0->setStyleSheet("QLabel{background-color:transparent;color:#ffffff;}");
    conLabel_0->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    conLabel_0->setGeometry(Tool::relRect(105,10,40,40));

    QLabel *conLabel_1=new QLabel(helpWidget);
    conLabel_1->setText("对不起，该功能尚未实现\n敬请期待！");
    conLabel_1->setAlignment(Qt::AlignCenter);
    conLabel_1->setStyleSheet("QLabel{background-color:#c62f2f;color:#000000;border-radius:6px;}");
    conLabel_1->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    conLabel_1->setGeometry(Tool::relRect(15,250,330,80));
}

void MainWindow::initConnectPageWidget()
{
    connectPageWidget=new PQWidget("",Tool::ButClass_ConnectPageWidget,centWidget);
    connectPageWidget->setGeometry(Tool::relRect(0,0,360,500));
    connectPageWidget->setStyleSheet("background-color:#fafafa;");
    connect(connectPageWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    connectButton=new ImageButton("",Tool::ButClass_ConnectButton,bottomBar);
    connectButton->setText("连接");
    connectButton->setStyleSheet(Tool::retStyleStr("#000000","#333333","#000000"));
    connectButton->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    connectButton->setGeometry(Tool::relRect(40,10,60,40));
    connect(connectButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    computerButton=new ImageButton("",Tool::ButClass_ComputerButton,bottomBar);
    computerButton->setText("计算机");
    computerButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    computerButton->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    computerButton->setGeometry(Tool::relRect(150,10,80,40));
    connect(computerButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    chatButton=new ImageButton("",Tool::ButClass_ChatButton,bottomBar);
    chatButton->setText("聊天");
    chatButton->setStyleSheet(Tool::retStyleStr("#ffffff","#333333","#000000"));
    chatButton->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    chatButton->setGeometry(Tool::relRect(270,10,60,40));
    connect(chatButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QLabel *nameBlock=new QLabel(topBar);
    nameBlock->setText("屏幕共享与控制器");
    nameBlock->setStyleSheet("QLabel{background-color:transparent;color:#ffffff;}");
    nameBlock->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    nameBlock->setGeometry(Tool::relRect(15,10,170,40));
    //nameBlock->adjustSize();

    PQWidget *centUpWidget=new PQWidget("",Tool::ButClass_CentUpWidget,connectPageWidget);
    centUpWidget->setGeometry(Tool::relRect(4,2,352,296));
    centUpWidget->setStyleSheet("background-color:#e4e4e4;border-radius:6px");
    connect(centUpWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));
    QLabel *labelGet=new QLabel(centUpWidget);
    labelGet->setText("获取对方屏幕或控制权限");
    labelGet->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    labelGet->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    labelGet->setGeometry(Tool::relRect(15,15,200,40));
    labelGet->adjustSize();
    QLabel *conLabel_0=new QLabel(centUpWidget);
    conLabel_0->setText("对方ID：");
    conLabel_0->setStyleSheet("QLabel{background-color:transparent;color:#c62f2f;}");
    conLabel_0->setFont(Tool::retFont("Timers" , Tool::fontSize(10) ,  QFont::DemiBold));
    conLabel_0->setGeometry(Tool::relRect(15,55,95,20));
    conLabel_0->adjustSize();
    QLabel *conLabel_4=new QLabel(centUpWidget);
    conLabel_4->setText("__________________________________________");
    conLabel_4->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_4->setFont(Tool::retFont("Timers" , Tool::fontSize(10) ,  QFont::Bold));
    conLabel_4->setGeometry(Tool::relRect(15,100,200,10));
    partnerIdInputBox=new NewLineEdit(Tool::ButClass_PartnerIdInputBox,centUpWidget);
    partnerIdInputBox->setStyleSheet("QLineEdit{ background-color:transparent;border-width:0;\
border-style:outset;}QLineEdit:focus{color:#c62f2f}");
    partnerIdInputBox->setGeometry(Tool::relRect(15,66,200,60));
    partnerIdInputBox->setAlignment(Qt::AlignLeft);
    partnerIdInputBox->setPlaceholderText("输入对方ID");
    partnerIdInputBox->setFont(Tool::retFont("Timers" , Tool::fontSize(15) ,  QFont::Bold));
    connect(partnerIdInputBox,SIGNAL(inFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    connect(partnerIdInputBox,SIGNAL(outFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    QLabel *conLabel_1=new QLabel(centUpWidget);
    conLabel_1->setText("对方密码：");
    conLabel_1->setStyleSheet("QLabel{background-color:transparent;color:#c62f2f;}");
    conLabel_1->setFont(Tool::retFont("Timers" , Tool::fontSize(10) ,  QFont::DemiBold));
    conLabel_1->setGeometry(Tool::relRect(15,127,95,20));
    conLabel_1->adjustSize();
    QLabel *conLabel_5=new QLabel(centUpWidget);
    conLabel_5->setText("__________________________________________");
    conLabel_5->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_5->setFont(Tool::retFont("Timers" , Tool::fontSize(10) ,  QFont::Bold));
    conLabel_5->setGeometry(Tool::relRect(15,176,200,10));
    partnerPasswdInputBox=new NewLineEdit(Tool::ButClass_PartnerPasswdInputBox,centUpWidget);
    partnerPasswdInputBox->setStyleSheet("QLineEdit{ background-color:transparent;border-width:0;\
border-style:outset;}QLineEdit:focus{color:#c62f2f}");
    partnerPasswdInputBox->setGeometry(Tool::relRect(15,142,200,60));
    partnerPasswdInputBox->setAlignment(Qt::AlignLeft);
    partnerPasswdInputBox->setPlaceholderText("输入对方密码");
    partnerPasswdInputBox->setFont(Tool::retFont("Timers" , Tool::fontSize(15) ,  QFont::Bold));
    connect(partnerPasswdInputBox,SIGNAL(inFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));
    connect(partnerPasswdInputBox,SIGNAL(outFocus(int,bool)),this,SLOT(changeMainPage(int,bool)));

    getSharedScreenButton=new ImageButton("",Tool::ButClass_GetSharedScreenButton,centUpWidget);
    getSharedScreenButton->setText("查看对方屏幕");
    getSharedScreenButton->setAlignment(Qt::AlignCenter);
    getSharedScreenButton->setStyleSheet(Tool::retStyleStr("#ffffff","#000000","#000000",
                                                           "#c62f2f","#c62f2f","#c62f2f"));
    getSharedScreenButton->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    getSharedScreenButton->setGeometry(Tool::relRect(15,220,150,40));
    //getSharedScreenButton->adjustSize();
    connect(getSharedScreenButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));
    controlRemoteDeviceButton=new ImageButton("",Tool::ButClass_ControlRemoteDeviceButton,centUpWidget);
    controlRemoteDeviceButton->setText("控制对方设备");
    controlRemoteDeviceButton->setAlignment(Qt::AlignCenter);
    controlRemoteDeviceButton->setStyleSheet(Tool::retStyleStr("#ffffff","#000000","#000000",
                                                               "#c62f2f","#c62f2f","#c62f2f"));
    controlRemoteDeviceButton->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    controlRemoteDeviceButton->setGeometry(Tool::relRect(191,220,150,40));
    //controlRemoteDeviceButton->adjustSize();
    connect(controlRemoteDeviceButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

//=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
//=======================================================
    testButton=new ImageButton("",Tool::ButClass_TestButton,centUpWidget);
    testButton->setText("测试功能的按钮");
    testButton->setAlignment(Qt::AlignCenter);
    testButton->setStyleSheet(Tool::retStyleStr("#ffffff","#000000","#000000",
                                                           "#023fbb","#023fbb","#023fbb"));
    testButton->setFont(Tool::retFont("Timers" , Tool::fontSize(24) ,  QFont::Bold));
    testButton->setGeometry(Tool::relRect(50,205,250,80));
    //testButton->adjustSize();
    connect(testButton,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));
//=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
    //=======================================================
//=======================================================

    PQWidget *centBotWidget=new PQWidget("",Tool::ButClass_CentBotWidget,connectPageWidget);
    centBotWidget->setGeometry(Tool::relRect(4,302,352,196));
    centBotWidget->setStyleSheet("background-color:#e4e4e4;border-radius:6px");
    connect(centBotWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));
    QLabel *labelShare=new QLabel(centBotWidget);
    labelShare->setText("分享屏幕或允许被远程控制");
    labelShare->adjustSize();
    labelShare->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    labelShare->setFont(Tool::retFont("Timers" , Tool::fontSize(14) ,  QFont::DemiBold));
    labelShare->setGeometry(Tool::relRect(15,15,170,40));
    labelShare->adjustSize();
    QLabel *conLabel_2=new QLabel(centBotWidget);
    conLabel_2->setText("您的ID：");
    conLabel_2->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_2->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    conLabel_2->setGeometry(Tool::relRect(15,68,110,40));
    //conLabel_2->adjustSize();
    userIDShower=new QLabel(centBotWidget);
    userIDShower->setText(userId);
    //userIDShower->setAlignment(Qt::AlignCenter);
    userIDShower->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    userIDShower->setFont(Tool::retFont("Timers" , Tool::fontSize(15) ,  QFont::Bold));
    userIDShower->setGeometry(Tool::relRect(135,65,180,40));
    QLabel *conLabel_3=new QLabel(centBotWidget);
    conLabel_3->setText("您的密码：");
    conLabel_3->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    conLabel_3->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    conLabel_3->setGeometry(Tool::relRect(15,123,110,40));
    //conLabel_3->adjustSize();
    userPasswdShower=new QLabel(centBotWidget);
    userPasswdShower->setText(userPasswd);
    //userPasswdShower->setAlignment(Qt::AlignCenter);
    userPasswdShower->setStyleSheet("QLabel{background-color:transparent;color:#000000;}");
    userPasswdShower->setFont(Tool::retFont("Timers" , Tool::fontSize(15) ,  QFont::Bold));
    userPasswdShower->setGeometry(Tool::relRect(135,120,180,40));
}
void MainWindow::initComputerPageWidget()
{
    computerPageWidget=new PQWidget("",Tool::ButClass_ComputerPageWidget,centWidget);
    computerPageWidget->setGeometry(Tool::relRect(0,0,360,500));
    computerPageWidget->setStyleSheet("background-color:#fafafa;");
    connect(computerPageWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QLabel *conLabel_0=new QLabel(computerPageWidget);
    conLabel_0->setText("对不起，该功能尚未实现\n敬请期待！");
    conLabel_0->setAlignment(Qt::AlignCenter);
    conLabel_0->setStyleSheet("QLabel{background-color:#c62f2f;color:#000000;border-radius:6px;}");
    conLabel_0->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    conLabel_0->setGeometry(Tool::relRect(15,170,330,80));
}
void MainWindow::initChatPageWidget()
{
    chatPageWidget=new PQWidget("",Tool::ButClass_ChatPageWidget,centWidget);
    chatPageWidget->setGeometry(Tool::relRect(0,0,360,500));
    chatPageWidget->setStyleSheet("background-color:#fafafa;");
    connect(chatPageWidget,SIGNAL(clicked(int,bool)),this,SLOT(changeMainPage(int,bool)));

    QLabel *conLabel_0=new QLabel(chatPageWidget);
    conLabel_0->setText("对不起，该功能尚未实现\n敬请期待！");
    conLabel_0->setAlignment(Qt::AlignCenter);
    conLabel_0->setStyleSheet("QLabel{background-color:#c62f2f;color:#000000;border-radius:6px;}");
    conLabel_0->setFont(Tool::retFont("Timers" , Tool::fontSize(13) ,  QFont::DemiBold));
    conLabel_0->setGeometry(Tool::relRect(15,170,330,80));
}
void MainWindow::networkError(QString err, int no)
{
    qDebug()<<"Faild number "<<no<<":"<<err;
}
void MainWindow::userID_Passwd_changed(QString iD, QString passwd)
{
    userIDShower->setText("<u>" + userId + "</u>");
    userPasswdShower->setText("<u>" + userPasswd + "</u>");
}
void MainWindow::weWereRequested()
{
    emit startConnectPartner_WhenWeWereRequested(&connectByPartnerHostAndShareScreen_Threader);
}
void MainWindow::exitApplication(int flag)
{//This function was called to send disconnect request toserver,and receive exit signal with flag=1.
    if(flag)
    {
        exit(0);
    }else
    {
        emit disConnectWithServer();
    }
}
void MainWindow::showScreenShotReceived(QByteArray recv_byte_buffer)
{
    if(isNewPicData)
    {
        int fl=0;
        memcpy(&fl, recv_byte_buffer.data(), INT_BYTE_LENGTH);
        if(fl != THIS_IS_PICTURE_DATA)
            return;

        memcpy(&picDataLength,recv_byte_buffer.data() + INT_BYTE_LENGTH, INT_BYTE_LENGTH);
        if(picDataBuffer)
            picDataBuffer->~QByteArray();
        picDataBuffer = new QByteArray(recv_byte_buffer.data() + INT_BYTE_LENGTH * 2);

        isNewPicData = false;

        qDebug()<<"This screenshot picture's size is "<<picDataLength;
    }else
    {
        picDataBuffer->append(recv_byte_buffer);
    }

    if(picDataLength > picDataBuffer->size())
    {
        return;
    }
    else// if(picDataLength <= picDataBuffer->size())
    {
        QByteArray picByte;
        picByte.resize(picDataLength);
        memcpy(picByte.data(), picDataBuffer->data(), picDataLength);

        QPixmap pic;
        pic.loadFromData(picByte);
        qDebug()<<"After load by QPixmap,the instance's size is "<<pic.size()<<picByte.size();

        screenSharedVideoWidget->setParent(rootWidget);
        screenSharedVideoWidget->setVisible(true);
        partnerScreenPictureShower->setIcon(QIcon(pic));

        isNewPicData = true;
    }
}
void MainWindow::yourPartnerHaveExit()
{
    stopFlag_ConnectPartnerHostAndGetScreen_Threader = false;
    screenSharedVideoWidget->setVisible(false);

    //tcpConnecterWithServer_Threader.start();
    //connectPartnerHostAndGetScreen_Threader.start();
}
void MainWindow::serverFinalPortChanged()
{
    serverPortInputBox->setText(QString::number(serverFinalPort));
}
