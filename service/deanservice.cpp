#include "deanservice.h"
#include "myjson.h"
#include "usettings.h"
#include "signaltransfer.h"
#include "accountinfo.h"
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
    // qDebug() << "收到消息:" << message;
    // 在这里处理接收到的消息
    parseRequiredMessage(MyJson(message.toUtf8()));
}

void DeanService::onBinaryMessageReceived(const QByteArray &message)
{
    // qDebug() << "收到二进制消息:" << message;
    // 在这里处理接收到的二进制消息
    MyJson json(message);
    parseReceivedMessage(json);
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

/**
 * 接收自己发过去的请求信息，走Text通道
 */
void DeanService::parseRequiredMessage(MyJson json)
{
    int code = json.i("code");
    QString msg = json.s("msg");
    
    if (code != 200)
    {
        qCritical() << "DeanService::parseRequiredMessage 错误:" << code << msg;
        return;
    }

    if (!requiredTypes.size())
    {
        qCritical() << "DeanService::parseRequiredMessage 所有消息已接收";
        return;
    }

    QString type = requiredTypes.takeFirst();
    if (type == "X0000") // 信息列表
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

                // 获取当前微信号的详细信息
                // getWxInfo(wxid); // 没必要，因为信息是重复的
            }
        }
    }
    else if (type == "Q0003") // 获取Hook微信账号信息
    {
        
    }
    else
    {
        qWarning() << "DeanService::parseRequiredMessage 未知类型:" << type;
    }
}

/**
 * 接收Dean发过来的事件消息，走Binary通道
 */
void DeanService::parseReceivedMessage(MyJson json)
{
    int event = json.i("event");
    QString wxid = json.s("wxid");
    MyJson data = json.o("data");
    if (event == 10014) // 账号变动事件
    {
        // 处理账号变动事件
    }
    else if (event == 10008 || event == 10009 || event == 10010) // 收到群聊/私聊/自己消息
    {
        // 处理收到的群聊消息
        /*{
            "event": 10008,
            "wxid": "wxid_nq6r0w9v12612",
            "data": {
                "type": "recvMsg",
                "des": "收到消息",
                "data": {
                    "timeStamp": "1716619844146",
                    "fromType": 2,
                    "msgType": 1,
                    "msgSource": 0,
                    "fromWxid": "34457730396@chatroom",
                    "finalFromWxid": "wxid_3sq4tklb6c3121",
                    "atWxidList": ["wxid_nq6r0w9v12612", "wxid_s4icha2bm2zg12"],
                    "silence": 0,
                    "membercount": 3,
                    "signature": "V1_1YiTEwOY|v1_1YiTEwOY",
                    "msg": "@忆白\\u2005@小鹿\\uD83D\\uDE00\\uD83D\\uDE00摸\\u2005你好\\uD83E\\uDD23世界",
                    "msgId": "7053704521966022857",
                    "msgBase64": "QOW\/hueZvVx1MjAwNUDlsI\/pub9cdUQ4M0RcdURFMDBcdUQ4M0RcdURFMDDmkbhcdTIwMDXkvaDlpb1cdUQ4M0VcdUREMjPkuJbnlYw="
                },
                "timestamp": "1716619844147",
                "wxid": "wxid_nq6r0w9v12612",
                "port": 8888,
                "pid": 3944,
                "flag": "7888"
            }
        }*/
        QString des = data.s("des");
        data = data.o("data");
        ChatBean chatBean;
        chatBean.des = des;
        chatBean.timeStamp = data.s("timeStamp").toLongLong();
        chatBean.fromType = data.i("fromType");
        chatBean.msgType = data.i("msgType");
        chatBean.msgSource = data.i("msgSource");
        chatBean.fromWxid = data.s("fromWxid");
        chatBean.finalFromWxid = data.s("finalFromWxid");
        chatBean.atWxidList = data.sl("atWxidList");
        chatBean.silence = data.i("silence");
        chatBean.membercount = data.i("membercount");
        chatBean.signature = data.s("signature");
        chatBean.msg = data.s("msg");
        chatBean.msgId = data.s("msgId");
        chatBean.msgBase64 = data.s("msgBase64");
        qInfo() << des << chatBean.getObjectId() << chatBean.getSenderId() << chatBean.getMsg();

        ac->addChat(chatBean);
        emit st->signalNewMessage(chatBean);
    }
    else if (event == 10006) // 转账事件
    {
        // 处理转账事件
    }
    else if (event == 10011) // 好友请求
    {
        // 处理好友请求
    }
    else if (event == 10007) // 支付事件
    {
        // 处理支付事件
    }
    else if (event == 10000) // 注入成功
    {
        // 处理注入成功事件
    }
    else
    {
        qWarning() << "DeanService::parseReceivedMessage 未知事件类型:" << event;
    }
}

void DeanService::getWxList()
{
    qInfo() << "获取Hook微信列表";
    sendApiMessage("X0000", MyJson());
}

void DeanService::getWxInfo(QString wxid)
{
    qInfo() << "获取Hook微信账号信息";
    MyJson json;
    json.insert("type", "1");
    sendApiMessage("Q0003", json.toBa());
}

void DeanService::sendUserMessage(QString wxid, QString msg)
{
    MyJson json;
    json.insert("wxid", wxid);
    json.insert("msg", msg);
    sendApiMessage("sendText", json.toBa());
}
