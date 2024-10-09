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
}

void USettings::setDeanServerUrl(QString url)
{

    this->_deanServerUrl = url;
}

QString USettings::deanServerUrl()
{
    return this->_deanServerUrl;
}

void USettings::setDeanWxid(QString wxid)
{
    this->_deanWxid = wxid;
}

QString USettings::deanWxid()
{
    return this->_deanWxid;
}

void USettings::setMaxMessageCount(int count)
{
    this->_maxMessageCount = count;
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

void USettings::setWhiteList_friendEnabled(QString wxid, bool enable)
{
    if (enable) {
        this->_whiteList_friendWxid.append(wxid);
    } else {
        this->_whiteList_friendWxid.removeAll(wxid);
    }
    this->setValue("us/whiteList_friendWxid", this->_whiteList_friendWxid.join(";"));
}

bool USettings::isWhiteList_friendEnabled(QString wxid)
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

void USettings::setWhiteList_groupEnabled(QString wxid, bool enable)
{
    if (enable) {
        this->_whiteList_groupWxid.append(wxid);
    } else {
        this->_whiteList_groupWxid.removeAll(wxid);
    }
    this->setValue("us/whiteList_groupWxid", this->_whiteList_groupWxid.join(";"));
}

bool USettings::isWhiteList_groupEnabled(QString wxid)
{
    return this->_whiteList_groupWxid.contains(wxid);
}

QStringList USettings::whiteList_groupWxid()
{
    return this->_whiteList_groupWxid;
}
