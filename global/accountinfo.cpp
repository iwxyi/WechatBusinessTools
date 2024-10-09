#include "accountinfo.h"
#include <QDebug>

AccountInfo *ac = new AccountInfo();

AccountInfo::AccountInfo(QObject *parent) : QObject(parent)
{

}

AccountInfo::~AccountInfo()
{
}

QString AccountInfo::getWxid()
{
    return wxid;
}

void AccountInfo::setWxid(QString wxid)
{
    this->wxid = wxid;
}

QString AccountInfo::getNick()
{
    return nick;
}

void AccountInfo::setNick(QString nick)
{
    this->nick = nick;
}

void AccountInfo::addChat(ChatBean chatBean)
{
    if (!chatMap.contains(chatBean.getObjectId()))
    {
        chatMap.insert(chatBean.getObjectId(), QList<ChatBean>());
    }
    chatMap[chatBean.getObjectId()].append(chatBean);
}

ChatBean AccountInfo::getLatestChat(QString wxid) const
{
    if (!chatMap.contains(wxid))
    {
        qWarning() << "获取最新消息失败，wxid:" << wxid;
        return ChatBean();
    }
    if (chatMap[wxid].isEmpty())
    {
        return ChatBean();
    }
    return chatMap[wxid].first();
}

const QList<ChatBean>& AccountInfo::getChatList(QString wxid) const
{
    if (!chatMap.contains(wxid))
    {
        qWarning() << "获取消息列表失败，wxid:" << wxid;
        return QList<ChatBean>();
    }
    return chatMap.value(wxid);
}

void AccountInfo::setFriendList(const QMap<QString, FriendBean> &friendMap)
{
    this->friendMap = friendMap;
}

void AccountInfo::setGroupList(const QMap<QString, GroupBean> &groupMap)
{
    this->groupMap = groupMap;
}

void AccountInfo::addFriend(const FriendBean &friendBean)
{
    friendMap.insert(friendBean.wxid, friendBean);
}

void AccountInfo::addGroup(const GroupBean &groupBean)
{
    groupMap.insert(groupBean.wxid, groupBean);
}

FriendBean AccountInfo::getFriend(QString wxid) const
{
    if (!friendMap.contains(wxid))
    {
        qWarning() << "获取好友信息失败，wxid:" << wxid;
        return FriendBean();
    }
    return friendMap.value(wxid);
}

GroupBean AccountInfo::getGroup(QString wxid) const
{
    if (!groupMap.contains(wxid))
    {
        qWarning() << "获取群信息失败，wxid:" << wxid;
        return GroupBean();
    }
    return groupMap.value(wxid);
}

void AccountInfo::setGroupMembers(QString groupId, QMap<QString, QString> groupMemberNickMap)
{
    if (!groupMap.contains(groupId))
    {
        qWarning() << "设置群成员信息失败，wxid:" << groupId;
        return;
    }
    groupMap[groupId].groupMemberNickMap = groupMemberNickMap;
}

QString AccountInfo::getGroupMemberNick(QString groupId, QString wxid)
{
    if (!groupMap.contains(groupId))
    {
        qWarning() << "获取群成员昵称所在群失败，wxid:" << groupId;
        return "";
    }
    if (!groupMap[groupId].groupMemberNickMap.contains(wxid))
    {
        qWarning() << "获取群成员昵称失败，wxid:" << wxid;
        return "";
    }
    return groupMap[groupId].groupMemberNickMap[wxid];
}

void AccountInfo::updateGroupChatsMemberNicks(QString groupId)
{
    if (!groupMap.contains(groupId))
    {
        qWarning() << "更新群成员昵称失败，wxid:" << groupId;
        return;
    }

    QMap<QString, QString> groupMemberNickMap = groupMap[groupId].groupMemberNickMap;
    QList<ChatBean>& chatList = chatMap[groupId];
    qInfo() << "更新群" << groupId << "中" << chatList.size() << "条消息的群成员昵称";
    for (ChatBean& chat : chatList)
    {
        if (!groupMemberNickMap.contains(chat.finalFromWxid))
        {
            qWarning() << "更新消息中的群成员昵称失败，wxid:" << chat.finalFromWxid;
            continue;
        }
        chat.senderName = groupMemberNickMap[chat.finalFromWxid];
    }
}
