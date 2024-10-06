#ifndef DEANSERVICE_H
#define DEANSERVICE_H

#include <QObject>
#include <QWebSocket>
#include "utils/myjson.h"

class DeanService : public QObject
{
    Q_OBJECT
public:
    explicit DeanService(QObject *parent = nullptr);

    void init();
    void start();

    void getWxList();
    void getWxInfo(QString wxid);
    void sendUserMessage(QString wxid, QString msg);

private:
    void sendApiMessage(QString wxid, QString type, MyJson data);
    void sendApiMessage(QString type, MyJson data);
    void parseRequiredMessage(MyJson json);
    void parseReceivedMessage(MyJson json);

signals:

public slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket *deanWs = nullptr;
    QStringList requiredTypes;

    QString nick;
};

#endif // DEANSERVICE_H
