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
    void signalGroupMemberListChanged(QString groupId);
    void signalGroupMemberNickChanged(QString groupId, QString userId, QString nick);
};

extern SignalTransfer* st;

#endif // SIGNALTRANSFER_H
