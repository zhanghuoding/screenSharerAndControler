#include "tool.h"

Tool::Tool()
{

}

QSize Tool::LogicalScreenSize(360,640);//用于进行尺寸换算的逻辑值

QSize Tool::size(int width, int height)
{
    int w=0,h=0;
    w=width*(screenSize->width()/(double)Tool::LogicalScreenSize.width())+0.5;//加0.5就是做四舍五入的运算
    h=height*(screenSize->height()/(double)Tool::LogicalScreenSize.height())+0.5;
    return QSize(w,h);
}
QRect Tool::absRect(int ax, int ay, int aw, int ah)
{
#ifdef DESKTOP

    /*
     * Because many phone has a tool bar on the screen top,
     * and the tool bar width is 20 dpi.
     * So,for each coordinate y should reduce 20.
     * When developing,we treat the height of phone 640-20=620,not 640,but treat height on computer 640.
     */
    ay+=20;
#endif
    return QRect(width(ax),height(ay),width(aw),height(ah));
}
QRect Tool::relRect(int ax, int ay, int aw, int ah)
{
    return QRect(width(ax),height(ay),width(aw),height(ah));
}
int Tool::width(int width)
{
    return width*(screenSize->width()/(double)Tool::LogicalScreenSize.width())+0.5;
}
int Tool::height(int height)
{
    return height*(screenSize->height()/(double)Tool::LogicalScreenSize.height())+0.5;
}
char *Tool::rateWidthToChar(int x)
{
    return QString::number(width(x)).toLatin1().data();
}
char *Tool::rateHeightToChar(int y)
{
    return QString::number(height(y)).toLatin1().data();
}
QSize Tool::reSize(int width, int height)
{
    int w=0,h=0;
    w=width/(screenSize->width()/(double)Tool::LogicalScreenSize.width())+0.5;//加0.5就是做四舍五入的运算
    h=height/(screenSize->height()/(double)Tool::LogicalScreenSize.height())+0.5;
    return QSize(w,h);
}
QRect Tool::reRect(int ax, int ay, int aw, int ah)
{
    return QRect(width(ax),height(ay),width(aw),height(ah));
}
int Tool::reWidth(int width)
{
    return width/(screenSize->width()/(double)Tool::LogicalScreenSize.width())+0.5;
}
int Tool::reHeight(int height)
{
    return height/(screenSize->height()/(double)Tool::LogicalScreenSize.height())+0.5;
}
int Tool::fontSize(int size)
{
    //return size*(screenSize->width()/(double)Tool::LogicalScreenSize.width())+0.5;
    /*
     * For every device,the size of the Font is equal,this is also said,the standard of Font is equal.
     * So,we needn't convert Font size according the rate of screen.
     */
    //return (int)(size*2.3+0.5);
    return size;
}
QString Tool::retStyleStr(QString color1, QString color2, QString color3,
                          QString backColor1,QString backColor2,QString backColor3)
{
    return "QLabel{background-color:" + backColor1 + ";color:" + color1
                   + ";}QLabel:hover{background-color:" + backColor2 + ";color:" + color2
                   + ";}QLabel:pressed{background-color:" + backColor3 + ";color:" + color3
                   + ";}";
}
QFont Tool::retFont(QString type, int size, int demi)
{
    if(size<=0)
        size=12;
    QFont font(type,size,demi);
    //font.setPointSize(1);
    //font.setPixelSize(Tool::fontSize(size));
    return font;
}
QString Tool::timeSign()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd hh:mm:ss dddd");
}
