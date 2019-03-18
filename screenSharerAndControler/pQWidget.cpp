#include "pQWidget.h"
#include <QStyleOption>
#include <QStylePainter>

PQWidget::PQWidget(QString freStr,int flag,QWidget *parent) : QWidget(parent)
{
    this->flag=flag;
    //this->setStyleSheet("QWidget{border-image: url("+freStr+")};");
}
void PQWidget::mousePressEvent(QMouseEvent *event)
{
    // 如果单击了就触发clicked信号
    if ( event->button() == Qt::LeftButton )
    {
        emit clicked(flag,true);
        qDebug()<<"is clicked!"<<flag;
    }
    // 将该事件传给父类处理
    //QLabel::mousePressEvent(event);
}
void PQWidget::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);

    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect=rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(event);
}
