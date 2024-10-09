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

    void setMaxMessageCount(int count);
    int maxMessageCount();

private:
    QString _deanServerUrl;
    QString _deanWxid;

    int _maxMessageCount;
};

extern USettings *us;

#endif // USETTINGS_H
