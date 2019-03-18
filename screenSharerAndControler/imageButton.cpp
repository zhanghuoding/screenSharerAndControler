#include "imageButton.h"

ImageButton::ImageButton(QString imagePath,int flag,QWidget *parent) : QLabel(parent)
{
    this->flag=flag;
    //this->setStyleSheet("QLabel{border-radius:60px}");
    this->setStyleSheet("QLabel{border-image: url("+imagePath+")};");
}
ImageButton::~ImageButton()
{
    ;
}
void ImageButton::mousePressEvent(QMouseEvent *event)
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
