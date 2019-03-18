#ifndef PQWIDGET_H
#define PQWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

class PQWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PQWidget(QString freStr,int flag=0,QWidget *parent = 0);


signals:
    void clicked(int flag,bool isClicked=true);
public slots:
    virtual void mousePressEvent ( QMouseEvent * event )Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event)Q_DECL_OVERRIDE;
private:
    int flag;
};

#endif // PQWIDGET_H
