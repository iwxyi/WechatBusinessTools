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
    ChatBean getLatestChat(QString wxid) const;
    const QList<ChatBean> &getChatList(QString wxid) const;

    void setFriendMap(const QMap<QString, FriendBean> &friendMap);
    void setGroupMap(const QMap<QString, GroupBean> &groupMap);
    QMap<QString, FriendBean> getFriendMap() const;
    QMap<QString, GroupBean> getGroupMap() const;
    QList<FriendBean> getFriendList() const;
    QList<GroupBean> getGroupList() const;
    void addFriend(const FriendBean &friendBean);
    void addGroup(const GroupBean &groupBean);
    FriendBean getFriend(QString wxid) const;
    GroupBean getGroup(QString wxid) const;
    void setGroupMembers(QString groupId, QMap<QString, QString> groupMemberNickMap);
    QString getGroupMemberNick(QString groupId, QString wxid);
    void updateGroupChatsMemberNicks(QString groupId);

private:
    QString wxid;
    QString nick;

    QMap<QString, FriendBean> friendMap; // 好友列表
    QMap<QString, GroupBean> groupMap; // 群列表
    QMap<QString, QList<ChatBean>> chatMap; // 所有消息列表
};

extern AccountInfo *ac;

#endif // ACCOUNTINFO_H
