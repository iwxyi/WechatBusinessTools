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

    void setWhiteList_friendWxid(QStringList wxidList);
    void setWhiteList_friendChecked(QString wxid, bool enable);
    bool isWhiteList_friendChecked(QString wxid);
    QStringList whiteList_friendWxid();

    void setWhiteList_groupWxid(QStringList wxidList);
    void setWhiteList_groupChecked(QString wxid, bool enable);
    bool isWhiteList_groupChecked(QString wxid);
    QStringList whiteList_groupWxid();

    void setUseWhiteOrBlackList(bool enable);
    bool useWhiteOrBlackList();
    bool isFriendEnabled(QString wxid);
    bool isGroupEnabled(QString wxid);

private:
    QString _deanServerUrl;
    QString _deanWxid;

    int _maxMessageCount;
    QStringList _whiteList_friendWxid;
    QStringList _whiteList_groupWxid;

    bool _useWhiteOrBlackList;
};

extern USettings *us;

#endif // USETTINGS_H
