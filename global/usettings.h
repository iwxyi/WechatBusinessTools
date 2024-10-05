#ifndef USETTINGS_H
#define USETTINGS_H

#include <settings.h>
class USettings : public Settings
{
public:
    USettings(QString path, QObject* parent = nullptr);

    void init();

    void setDeanServerUrl(QString url);
    QString deanServerUrl();

private:
    QString _deanServerUrl;
};

extern USettings *us;

#endif // USETTINGS_H
