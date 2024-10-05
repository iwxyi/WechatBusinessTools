#include "usettings.h"

USettings *us = new USettings("settings.ini");

USettings::USettings(QString path, QObject* parent) : Settings(path)
{
    init();
}

void USettings::init()
{
    this->_deanServerUrl = this->getStr("deanServerUrl", "ws://192.168.0.103:7778");
}

void USettings::setDeanServerUrl(QString url)
{

    this->_deanServerUrl = url;
}

QString USettings::deanServerUrl()
{
    return this->_deanServerUrl;
}
