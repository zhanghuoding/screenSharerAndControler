#include "newLineEdit.h"

NewLineEdit::NewLineEdit(int flag,QWidget *parent) : QLineEdit(parent)
{
    this->flag=flag;
}
NewLineEdit::NewLineEdit(int flag,const QString &str, QWidget *parent) : QLineEdit(str,parent)
{
    this->flag=flag;
}
NewLineEdit::~NewLineEdit()
{

}
void NewLineEdit::focusInEvent(QFocusEvent *e)
{
    emit inFocus(flag,true);
    qDebug()<<"is clicked!"<<flag;
    QLineEdit::focusInEvent(e);
}
void NewLineEdit::focusOutEvent(QFocusEvent *e)
{
    emit outFocus(flag,true);
    qDebug()<<"is clicked!"<<flag;
    QLineEdit::focusOutEvent(e);
}
