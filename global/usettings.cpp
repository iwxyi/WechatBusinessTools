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