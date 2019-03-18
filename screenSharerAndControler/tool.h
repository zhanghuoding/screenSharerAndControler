#ifndef TOOL_H
#define TOOL_H

#include <QSize>
#include <QString>
#include <QRect>
#include <QFont>
#include <QDateTime>

#include "variable.h"


class Tool
{
public:
    enum {ButClass_ConvertionalWidget,ButClass_ConnectPageWidget,ButClass_ComputerPageWidget,\
        ButClass_ChatPageWidget,ButClass_ConnectButton,ButClass_ComputerButton,ButClass_ChatButton,\
          ButClass_MenuButton,ButClass_SettingButton,ButClass_HelpButton,ButClass_TopBar,\
          ButClass_CentWidget,ButClass_BottomBar,ButClass_CentUpWidget,ButClass_CentBotWidget,\
          ButClass_GetSharedScreenButton,ButClass_ControlRemoteDeviceButton,ButClass_ScreenSharedVideoWidget,\
          ButClass_SettingWidget,ButClass_HelpWidget,ButClass_BackFromSetting,ButClass_BackFromHelp,\
          ButClass_PartnerIdInputBox,ButClass_PartnerPasswdInputBox,ButClass_SettingPageEnterButton,\
         ButClass_ServerIPInputBox,ButClass_ServerPortInputBox,ButClass_TestButton};
    static QSize LogicalScreenSize;

    Tool();
    static QSize size(int width,int height);//计算并返回一个与计算机屏幕成一定比例的尺寸
    static QRect absRect(int ax=0, int ay=0, int aw=0, int ah=0);//返回一个与16:9屏幕成比例的图形的绝对位置和尺寸
    static QRect relRect(int ax=0, int ay=0, int aw=0, int ah=0);//返回一个与16:9屏幕成比例的图形的相对于父组件的相对位置和尺寸
    static int width(int width);//根据计算机屏幕的水平宽度，返回一个成比例的整型值
    static int height(int height);//根据计算机屏幕的竖直高度，返回一个成比例的整型值
    static char *rateWidthToChar(int x);//将传入的x按屏幕的宽比例换算后，转换成字符串返回
    static char *rateHeightToChar(int y);//将传入的y按屏幕的高比例换算后，转换成字符串返回
    static int fontSize(int size);//按照屏幕的宽来按比例计算字体的大小


    static QSize reSize(int width,int height);//计算并返回一个与计算机屏幕成一定比例的尺寸
    static QRect reRect(int ax=0, int ay=0, int aw=0, int ah=0);//返回一个与16:9屏幕成比例的图形的位置和尺寸
    static int reWidth(int width);//根据计算机屏幕的水平宽度，返回一个成比例的整型值
    static int reHeight(int height);//根据计算机屏幕的竖直高度，返回一个成比例的整型值

    static QString retStyleStr(QString color1,QString color2,QString color3,
                               QString backColor1="transparent",QString backColor2="transparent",
                               QString backColor3="transparent");

    static QFont retFont(QString type="Timers",int size=-1,int demi=QFont::Bold);
    static QString timeSign();
};

#endif // TOOL_H
