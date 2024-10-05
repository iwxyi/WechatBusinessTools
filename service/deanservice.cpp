#include "deanservice.h"
#include "myjson.h"
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
}

void DeanService::onDisconnected()
{
    qDebug() << "WebSocket已断开连接";
}

void DeanService::onTextMessageReceived(const QString &message)
{
    qDebug() << "收到消息:" << message;
    // 在这里处理接收到的消息
}

void DeanService::onBinaryMessageReceived(const QByteArray &message)
{
    // qDebug() << "收到二进制消息:" << message;
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

