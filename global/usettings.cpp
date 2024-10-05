#include "usettings.h"

USettings *us = new USettings("settings.ini");

USettings::USettings(QString path, QObject* parent) : Settings(path)
{

}
