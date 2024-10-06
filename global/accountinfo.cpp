#include "accountinfo.h"

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
    chatMap.insert(chatBean.getObjectId(), chatBean);
}

void AccountInfo::setFriendList(QMap<QString, FriendBean> friendMap)
{
    this->friendMap = friendMap;
}

void AccountInfo::setGroupList(QMap<QString, GroupBean> groupMap)
{
    this->groupMap = groupMap;
}

void AccountInfo::addFriend(FriendBean friendBean)
{
    friendMap.insert(friendBean.wxid, friendBean);
}

void AccountInfo::addGroup(GroupBean groupBean)
{
    groupMap.insert(groupBean.wxid, groupBean);
}
