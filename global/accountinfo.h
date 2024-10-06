#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QObject>
#include <QMap>
#include "chatbean.h"

class AccountInfo : public QObject
{
    Q_OBJECT
public:
    AccountInfo(QObject *parent = nullptr);
    ~AccountInfo();

    QString getWxid();
    void setWxid(QString wxid);
    QString getNick();
    void setNick(QString nick);

    void addChat(ChatBean chatBean);

private:
    QString wxid;
    QString nick;

    QMap<QString, ChatBean> chatMap; // 所有消息列表
};

extern AccountInfo *ac;

#endif // ACCOUNTINFO_H
