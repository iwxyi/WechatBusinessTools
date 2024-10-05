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
    void setDeanWxid(QString wxid);
    QString deanWxid();

private:
    QString _deanServerUrl;
    QString _deanWxid;
};

extern USettings *us;

#endif // USETTINGS_H
