#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "variable.h"
#include "type.h"

class ImageButton : public QLabel
{
    Q_OBJECT
public:
    explicit ImageButton(QString imagePath,int flag=0,QWidget *parent = 0);
    ~ImageButton();

signals:
    void clicked(int flag,bool isClicked=true);
public slots:
    virtual void mousePressEvent ( QMouseEvent * event )Q_DECL_OVERRIDE;
private:
    int flag;
};

#endif // IMAGEBUTTON_H
