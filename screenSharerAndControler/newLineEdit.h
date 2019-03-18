#ifndef NEWLINEEDIT_H
#define NEWLINEEDIT_H

#include <QLineEdit>
#include <QDebug>

class NewLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit NewLineEdit(int flag,QWidget *parent = nullptr);
    explicit NewLineEdit(int flag,const QString &str, QWidget *parent = Q_NULLPTR);
    ~NewLineEdit();

private:
    int flag;

signals:
    void inFocus(int flag,bool isInFocus=true);
    void outFocus(int flag,bool isOutFocus=true);

public slots:
    virtual void focusInEvent(QFocusEvent *e)Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent *e)Q_DECL_OVERRIDE;
};

#endif // NEWLINEEDIT_H
