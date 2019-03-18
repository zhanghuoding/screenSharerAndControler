#ifndef CONTROLREMOTEDEVICE_H
#define CONTROLREMOTEDEVICE_H

#include <QObject>

class ControlRemoteDevice : public QObject
{
    Q_OBJECT
public:
    explicit ControlRemoteDevice(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CONTROLREMOTEDEVICE_H