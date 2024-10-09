#ifndef USETTINGS_H
#define USETTINGS_H

#include <settings.h>
class USettings : public Settings
{
    Q_OBJECT
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
    void setWhiteList_friendChecked(QString wxid, bool check);
    bool isWhiteList_friendChecked(QString wxid);
    QStringList whiteList_friendWxid();

    void setWhiteList_groupWxid(QStringList wxidList);
    void setWhiteList_groupChecked(QString wxid, bool check);
    bool isWhiteList_groupChecked(QString wxid);
    QStringList whiteList_groupWxid();

    void setUseWhiteOrBlackList_friend(bool enable);
    bool useWhiteOrBlackList_friend();
    void setUseWhiteOrBlackList_group(bool enable);
    bool useWhiteOrBlackList_group();
    bool isFriendEnabled(QString wxid);
    bool isGroupEnabled(QString wxid);

signals:
    void signalFriendEnabledChanged(QString wxid, bool enabled);
    void signalGroupEnabledChanged(QString wxid, bool enabled);

private:
    QString _deanServerUrl;
    QString _deanWxid;

    int _maxMessageCount;
    QStringList _whiteList_friendWxid;
    QStringList _whiteList_groupWxid;
    bool _useWhiteOrBlackList_friend;
    bool _useWhiteOrBlackList_group;
};

extern USettings *us;

#endif // USETTINGS_H
