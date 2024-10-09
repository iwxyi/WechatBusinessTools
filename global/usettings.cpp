#include "usettings.h"

USettings *us = new USettings("settings.ini");

USettings::USettings(QString path, QObject* parent) : Settings(path)
{
    init();
}

void USettings::init()
{
    this->_deanServerUrl = this->getStr("us/deanServerUrl", "ws://192.168.0.103:7778");
    this->_deanWxid = this->getStr("us/deanWxid", "");
    this->_maxMessageCount = this->getInt("us/maxMessageCount", 20);
    this->_whiteList_friendWxid = this->getStr("us/whiteList_friendWxid").split(";");
    this->_whiteList_groupWxid = this->getStr("us/whiteList_groupWxid").split(";");
    this->_useWhiteOrBlackList_friend = this->getBool("us/useWhiteOrBlackList_friend", true);
    this->_useWhiteOrBlackList_group = this->getBool("us/useWhiteOrBlackList_group", true);
}

void USettings::setDeanServerUrl(QString url)
{
    this->_deanServerUrl = url;
    this->setValue("us/deanServerUrl", url);
}

QString USettings::deanServerUrl()
{
    return this->_deanServerUrl;
}

void USettings::setDeanWxid(QString wxid)
{
    this->_deanWxid = wxid;
    this->setValue("us/deanWxid", wxid);
}

QString USettings::deanWxid()
{
    return this->_deanWxid;
}

void USettings::setMaxMessageCount(int count)
{
    this->_maxMessageCount = count;
    this->setValue("us/maxMessageCount", count);
}

int USettings::maxMessageCount()
{
    return this->_maxMessageCount;
}

void USettings::setWhiteList_friendWxid(QStringList wxidList)
{
    this->_whiteList_friendWxid = wxidList;
    this->setValue("us/whiteList_friendWxid", wxidList.join(";"));
}

void USettings::setWhiteList_friendChecked(QString wxid, bool check)
{
    if (check) {
        this->_whiteList_friendWxid.append(wxid);
    } else {
        this->_whiteList_friendWxid.removeAll(wxid);
    }
    this->setValue("us/whiteList_friendWxid", this->_whiteList_friendWxid.join(";"));
    emit signalFriendEnabledChanged(wxid, this->_useWhiteOrBlackList_friend ? check : !check);
}

bool USettings::isWhiteList_friendChecked(QString wxid)
{
    return this->_whiteList_friendWxid.contains(wxid);
}

QStringList USettings::whiteList_friendWxid()
{
    return this->_whiteList_friendWxid;
}

void USettings::setWhiteList_groupWxid(QStringList wxidList)
{
    this->_whiteList_groupWxid = wxidList;
    this->setValue("us/whiteList_groupWxid", wxidList.join(";"));
}

void USettings::setWhiteList_groupChecked(QString wxid, bool check)
{
    if (check) {
        this->_whiteList_groupWxid.append(wxid);
    } else {
        this->_whiteList_groupWxid.removeAll(wxid);
    }
    this->setValue("us/whiteList_groupWxid", this->_whiteList_groupWxid.join(";"));
    emit signalGroupEnabledChanged(wxid, this->_useWhiteOrBlackList_group ? check : !check);
}

bool USettings::isWhiteList_groupChecked(QString wxid)
{
    return this->_whiteList_groupWxid.contains(wxid);
}

QStringList USettings::whiteList_groupWxid()
{
    return this->_whiteList_groupWxid;
}

void USettings::setUseWhiteOrBlackList_friend(bool enable)
{
    this->_useWhiteOrBlackList_friend = enable;
    this->setValue("us/useWhiteOrBlackList_friend", enable);
    emit signalFriendEnabledChanged("", enable);
}

bool USettings::useWhiteOrBlackList_friend()
{
    return this->_useWhiteOrBlackList_friend;
}

void USettings::setUseWhiteOrBlackList_group(bool enable)
{
    this->_useWhiteOrBlackList_group = enable;
    this->setValue("us/useWhiteOrBlackList_group", enable);
    emit signalGroupEnabledChanged("", enable);
}

bool USettings::useWhiteOrBlackList_group()
{
    return this->_useWhiteOrBlackList_group;
}

bool USettings::isFriendEnabled(QString wxid)
{
    bool checked = _whiteList_friendWxid.contains(wxid);
    return _useWhiteOrBlackList_friend ? checked : !checked;
}

bool USettings::isGroupEnabled(QString wxid)
{
    bool checked = _whiteList_groupWxid.contains(wxid);
    return _useWhiteOrBlackList_group ? checked : !checked;
}
