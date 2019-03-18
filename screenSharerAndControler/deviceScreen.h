#ifndef DEVICESCREEN_H
#define DEVICESCREEN_H

#include <QObject>
//#include <QAndroidJniObject>
#include <QList>
#include <QString>

class DeviceScreen : public QObject
{
    Q_OBJECT
public:
    explicit DeviceScreen(QObject *parent = nullptr);
    ~DeviceScreen();

    Q_PROPERTY(double dentisty READ dentisty WRITE setDentisty NOTIFY dentistyChanged)

    //获取屏幕像素密度
    Q_INVOKABLE double getDentisy();

    double dentisty()
    {
        return this->m_dentisty;
    }
    void setDentisty(double a)
    {
        this->m_dentisty=a;
    }

signals:
    void dentistyChanged();

public slots:
//    void onDentistyChanged
private:
    double m_dentisty;
};
#endif // DEVICESCREEN_H
