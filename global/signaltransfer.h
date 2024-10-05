#ifndef SIGNALTRANSFER_H
#define SIGNALTRANSFER_H

#include <QObject>
#include <QString>

class SignalTransfer : public QObject
{
    Q_OBJECT
signals:
    void signalWxidChanged(QString wxid);
    void signalNickChanged(QString nick);
};

extern SignalTransfer* st;

#endif // SIGNALTRANSFER_H
