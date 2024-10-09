#ifndef DEANSERVICE_H
#define DEANSERVICE_H

#include <QObject>
#include <QWebSocket>
#include "utils/myjson.h"

struct RequiredData
{
    QString wxid;
    QString type;
    MyJson data;

    RequiredData(QString wxid, QString type, MyJson data)
    {
        this->wxid = wxid;
        this->type = type;
        this->data = data;
    }
};

class DeanService : public QObject
{
    Q_OBJECT
public:
    explicit DeanService(QObject *parent = nullptr);

    void init();
    void start();

    void getWxList();
    void getWxInfo(QString wxid);
    void getFriendList();
    void getGroupList();
    void getGroupMemberList(QString groupId);
    void sendUserMessage(QString wxid, QString msg);

private:
    void sendApiMessage(QString wxid, QString type, MyJson data);
    void sendApiMessage(QString type, MyJson data);
    void parseRequiredMessage(MyJson json);
    void parseReceivedMessage(MyJson json);

signals:
    void signalRequiredMessage();

public slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);
    void wsSendTextMessageByQueue();

private:
    QWebSocket *deanWs = nullptr;

    QList<RequiredData> requiredQueue;
};

#endif // DEANSERVICE_H
