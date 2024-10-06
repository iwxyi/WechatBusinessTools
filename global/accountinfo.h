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
    void setFriendList(QMap<QString, FriendBean> friendMap);
    void setGroupList(QMap<QString, GroupBean> groupMap);
    void addFriend(FriendBean friendBean);
    void addGroup(GroupBean groupBean);

private:
    QString wxid;
    QString nick;

    QMap<QString, FriendBean> friendMap; // 好友列表
    QMap<QString, GroupBean> groupMap; // 群列表
    QMap<QString, ChatBean> chatMap; // 所有消息列表
};

extern AccountInfo *ac;

#endif // ACCOUNTINFO_H
