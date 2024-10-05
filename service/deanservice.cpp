#include "deanservice.h"
#include "myjson.h"
#include "usettings.h"
#include "signaltransfer.h"
#include <QDebug>

DeanService::DeanService(QObject *parent) : QObject(parent)
{
}

void DeanService::init()
{
    deanWs = new QWebSocket();
    connect(deanWs, &QWebSocket::connected, this, &DeanService::onConnected);
    connect(deanWs, &QWebSocket::disconnected, this, &DeanService::onDisconnected);
    connect(deanWs, &QWebSocket::textMessageReceived, this, &DeanService::onTextMessageReceived);
    connect(deanWs, &QWebSocket::binaryMessageReceived, this, &DeanService::onBinaryMessageReceived);
    connect(deanWs, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(deanWs, &QWebSocket::sslErrors, this, &DeanService::onSslErrors);
}

void DeanService::onConnected()
{
    qDebug() << "WebSocket已连接";
    getWxList();
}

void DeanService::onDisconnected()
{
    qDebug() << "WebSocket已断开连接";
}

void DeanService::onTextMessageReceived(const QString &message)
{
    qDebug() << "收到消息:" << message;
    // 在这里处理接收到的消息
    parseReceiveMessage(MyJson(message.toUtf8()));
}

void DeanService::onBinaryMessageReceived(const QByteArray &message)
{
    qDebug() << "收到二进制消息:" << message;
    // 在这里处理接收到的二进制消息
    MyJson json(message);
    qDebug() << "事件：" << json;
}

void DeanService::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket错误:" << error;
}

void DeanService::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "WebSocket SSL错误:" << errors;
}

void DeanService::start()
{
    if (deanWs) {
        deanWs->open(QUrl("ws://192.168.0.103:7778"));
    }
}

void DeanService::sendApiMessage(QString wxid, QString type, MyJson data)
{
    MyJson json;
    json.insert("type", type);
    json.insert("wxid", wxid);
    json.insert("data", data);
    requiredTypes.append(type);
    deanWs->sendTextMessage(json.toBa());
}

void DeanService::sendApiMessage(QString type, MyJson data)
{
    sendApiMessage(us->deanWxid(), type, data.toBa());
}

void DeanService::parseReceiveMessage(MyJson json)
{
    int code = json.i("code");
    QString msg = json.s("msg");
    
    if (code != 200)
    {
        qCritical() << "DeanService::parseReceiveMessage 错误:" << code << msg;
        return;
    }

    if (!requiredTypes.size())
    {
        qCritical() << "DeanService::parseReceiveMessage 所有消息已接收";
        return;
    }

    QString type = requiredTypes.takeFirst();
    if (type == "X0000")
    {
        QJsonArray result = json.a("result");
        for (int i = 0; i < result.size(); i++)
        {
            QJsonObject item = result.at(i).toObject();
            QString wxid = item.value("wxid").toString();
            QString nick = item.value("nick").toString();
            QString port = item.value("port").toString();

            // 设置默认的Hook微信
            if (us->deanWxid().isEmpty())
            {
                us->setDeanWxid(wxid);
                us->setValue("us/deanWxid", wxid);
                qInfo() << "设置默认的Hook微信:" << us->deanWxid() << nick;
                emit st->signalWxidChanged(us->deanWxid());
            }

            // 设置Hook微信昵称
            if (us->deanWxid() == wxid)
            {
                this->nick = nick;
                qInfo() << "账号信息：ID:" << us->deanWxid() << ",昵称:" << nick << ",端口:" << port;
                emit st->signalNickChanged(nick);
            }
        }
    }
}

void DeanService::getWxList()
{
    qInfo() << "获取Hook微信列表";
    sendApiMessage("X0000", MyJson());
}

void DeanService::getWxInfo(QString wxid)
{
    MyJson json;
    json.insert("event", "getWxInfo");
    json.insert("wxid", wxid);
    deanWs->sendTextMessage(json.toBa());
}

void DeanService::sendUserMessage(QString wxid, QString msg)
{
    MyJson json;
    json.insert("wxid", wxid);
    json.insert("msg", msg);
    sendApiMessage("sendText", json.toBa());
}
