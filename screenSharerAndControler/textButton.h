#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "variable.h"
#include "type.h"

class TextButton : public QLabel
{
    Q_OBJECT
public:
    explicit TextButton(QString text,int flag=0,QWidget *parent = 0);
    ~TextButton();

signals:
    void clicked(int flag,bool isClicked=true);
public slots:
    virtual void mousePressEvent ( QMouseEvent * event )Q_DECL_OVERRIDE;
private:
    int flag;
};

#endif // TEXTBUTTON_H
