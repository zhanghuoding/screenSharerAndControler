#include "textButton.h"

TextButton::TextButton(QString text,int flag,QWidget *parent) : QLabel(parent)
{
    this->flag=flag;
    //this->setStyleSheet(QString("QLabel{border-radius:60px}"));
    //this->setStyleSheet(QString("QLabel{border-image: url("+imagePath+")};"));
    this->setText(text);
    this->setTextFormat(Qt::RichText);
}
TextButton::~TextButton()
{
    ;
}
void TextButton::mousePressEvent(QMouseEvent *event)
{
    // 如果单击了就触发clicked信号
    if ( event->button() == Qt::LeftButton )
    {
        emit clicked(flag,true);
        qDebug()<<"is clicked!"<<flag;
    }
    // 将该事件传给父类处理
    QLabel::mousePressEvent(event);
}
