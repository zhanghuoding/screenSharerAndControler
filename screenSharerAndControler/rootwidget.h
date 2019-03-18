#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H

#include <QWidget>
#include <QStylePainter>
#include <QStyleOption>

class RootWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RootWidget(QWidget *parent = nullptr);

signals:

public slots:
    virtual void paintEvent(QPaintEvent *event)Q_DECL_OVERRIDE;
};

#endif // ROOTWIDGET_H
