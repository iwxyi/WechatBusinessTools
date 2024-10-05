#include "deanservice.h"
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

void DeanService::start()
{
    if (deanWs) {
        deanWs->open(QUrl("ws://192.168.0.103:7778"));
    }
}

