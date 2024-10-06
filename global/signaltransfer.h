#ifndef SIGNALTRANSFER_H
#define SIGNALTRANSFER_H

#include <QObject>
#include <QString>
#include "chatbean.h"

class SignalTransfer : public QObject
{
    Q_OBJECT
signals:
    void signalWxidChanged(QString wxid);
    void signalNickChanged(QString nick);
    void signalNewMessage(ChatBean msg);
};

extern SignalTransfer* st;

#endif // SIGNALTRANSFER_H
