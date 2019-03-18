#include "rootwidget.h"

RootWidget::RootWidget(QWidget *parent) : QWidget(parent)
{

}
void RootWidget::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);

    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect=rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(event);
}
