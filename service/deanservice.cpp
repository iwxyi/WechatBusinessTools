#include "deanservice.h"
#include "myjson.h"
#include "usettings.h"
#include "signaltransfer.h"
#include "accountinfo.h"
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#define WSQ_DEB if (0) qDebug()

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

    connect(this, &DeanService::signalRequiredMessage, this, &DeanService::wsSendTextMessageByQueue);
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
    WSQ_DEB << "DeanService::sendApiMessage 添加队列：" << type << wxid;
    requiredQueue.append(RequiredData(wxid, type, data));
    
    // 如果只有一个请求，则直接发送
    // 否则等待接收的信号
    if (requiredQueue.size() == 1)
    {
        wsSendTextMessageByQueue();
    }
    else
    {
        WSQ_DEB << "DeanService::sendApiMessage 等待上一个请求（" << requiredQueue.first().type << "）结束";
    }
}

void DeanService::sendApiMessage(QString type, MyJson data)
{
    sendApiMessage(us->deanWxid(), type, data.toBa());
}

void DeanService::wsSendTextMessageByQueue()
{
    if (requiredQueue.size() == 0)
    {
        qWarning() << "DeanService::wsSendTextMessageByQueue 没有请求";
        return;
    }

    RequiredData requiredData = requiredQueue.first();
    WSQ_DEB << "DeanService::wsSendTextMessageByQueue 发送请求：" << requiredData.type << ", 当前总请求:" << requiredQueue.size();
    
    MyJson json;
    json.insert("type", requiredData.type);
    json.insert("wxid", requiredData.wxid);
    json.insert("data", requiredData.data);
    deanWs->sendTextMessage(json.toBa());
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

    Q_ASSERT(requiredQueue.size());

    RequiredData requiredData = requiredQueue.takeFirst();

    // 如果还有请求，则继续发送
    WSQ_DEB << "DeanService::parseRequiredMessage 当前请求结束，剩余：" << requiredQueue.size();
    if (requiredQueue.size() > 0)
    {
        emit signalRequiredMessage();
    }
    
    // 解析数据
    QString type = requiredData.type;
    WSQ_DEB << "DeanService::parseRequiredMessage 响应回复：" << type << ", 剩余请求:" << requiredQueue.size();
    // WSQ_DEB << (json.toBa().left(500));
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
                getFriendList();
                getGroupList();
            }
        }
    }
    else if (type == "Q0003") // 获取Hook微信账号信息
    {
        
    }
    else if (type == "Q0005") // 获取Hook好友列表
    {
        /*{
            "code": 200,
            "msg": "操作成功",
            "result": [
                {
                    "wxid": "wxid_s4icha2bm2zg12",
                    "wxNum": "ebaiwl",
                    "nick": "忆白",
                    "remark": "",
                    "nickBrief": "",
                    "nickWhole": "",
                    "remarkBrief": "",
                    "remarkWhole": "",
                    "enBrief": "",
                    "enWhole": "",
                    "v3": "v3_020b3826fd03010000000000d62217f63836d8000000501",
                    "sign": "",
                    "country": "",
                    "province": "",
                    "city": "",
                    "momentsBackgroudImgUrl": "",
                    "avatarMinUrl": "",
                    "avatarMaxUrl": "",
                    "sex": "0"
                },
                .........
            ],
            "wxid": "wxid_nq6r0w9v12612",
            "port": 8888,
            "pid": 30788,
            "flag": "7888",
            "timestamp": "1716629212103"
        }*/

       ac->setFriendList(QMap<QString, FriendBean>());
       QJsonArray result = json.a("result");
       qInfo() << "获取好友列表" << result.size() << "个";
       
       for (int i = 0; i < result.size(); i++)
       {
           MyJson item = result.at(i).toObject();
           FriendBean friendBean;
           friendBean.wxid = item.s("wxid");
           friendBean.wxNum = item.s("wxNum");
           friendBean.nick = item.s("nick");
           friendBean.remark = item.s("remark");
           friendBean.nickBrief = item.s("nickBrief");
           friendBean.nickWhole = item.s("nickWhole");
           friendBean.remarkBrief = item.s("remarkBrief");
           friendBean.remarkWhole = item.s("remarkWhole");
           friendBean.enBrief = item.s("enBrief");
           friendBean.enWhole = item.s("enWhole");
           friendBean.v3 = item.s("v3");
           friendBean.sign = item.s("sign");
           friendBean.country = item.s("country");
           friendBean.province = item.s("province");
           friendBean.city = item.s("city");
           friendBean.momentsBackgroudImgUrl = item.s("momentsBackgroudImgUrl");
           friendBean.avatarMinUrl = item.s("avatarMinUrl");
           friendBean.avatarMaxUrl = item.s("avatarMaxUrl");
           friendBean.sex = item.s("sex");
           ac->addFriend(friendBean);
           // qDebug() << "添加好友" << friendBean.wxid << friendBean.getName();
       }
    }
    else if (type == "Q0006") // 获取Hook群聊列表
    {
        /*{
            "code": 200,
            "msg": "操作成功",
            "result": [
                {
                    "wxid": "34457730396@chatroom",
                    "wxNum": "",
                    "nick": "千寻Pro测试群",
                    "remark": "",
                    "nickBrief": "",
                    "nickWhole": "",
                    "remarkBrief": "",
                    "remarkWhole": "",
                    "enBrief": "",
                    "enWhole": "",
                    "v3": "",
                    "sign": "",
                    "country": "",
                    "province": "",
                    "city": "",
                    "momentsBackgroudImgUrl": "",
                    "avatarMinUrl": "",
                    "avatarMaxUrl": "",
                    "sex": "0",
                    "groupMemberNum": 3,
                    "groupManger": "wxid_3sq4tklb6c3121"
                }
            ],
            "wxid": "wxid_nq6r0w9v12612",
            "port": 7955,
            "pid": 7816,
            "flag": "7888",
            "timestamp": "1716555864760"
        }*/
        ac->setGroupList(QMap<QString, GroupBean>());
        QJsonArray result = json.a("result");
        qInfo() << "获取群聊列表" << result.size() << "个";

        for (int i = 0; i < result.size(); i++)
        {
            MyJson item = result.at(i).toObject();
            GroupBean groupBean;
            groupBean.wxid = item.s("wxid");
            groupBean.wxNum = item.s("wxNum");
            groupBean.nick = item.s("nick");
            groupBean.remark = item.s("remark");
            groupBean.nickBrief = item.s("nickBrief");
            groupBean.nickWhole = item.value("nickWhole").toString();
            groupBean.remarkBrief = item.s("remarkBrief");
            groupBean.remarkWhole = item.s("remarkWhole");
            groupBean.enBrief = item.s("enBrief");
            groupBean.enWhole = item.s("enWhole");
            groupBean.v3 = item.s("v3");
            groupBean.sign = item.s("sign");
            groupBean.country = item.s("country");
            groupBean.province = item.s("province");
            groupBean.city = item.value("city").toString();
            groupBean.momentsBackgroudImgUrl = item.s("momentsBackgroudImgUrl");
            groupBean.avatarMinUrl = item.s("avatarMinUrl");
            groupBean.avatarMaxUrl = item.s("avatarMaxUrl");
            groupBean.sex = item.s("sex");
            groupBean.groupMemberNum = item.i("groupMemberNum");
            groupBean.groupManger = item.s("groupManger");
            ac->addGroup(groupBean);
            // qDebug() << "添加群" << groupBean.wxid << groupBean.getName();
        }
    }
    else if (type == "Q0008") // 获取Hook微信群成员列表
    {
        QJsonArray result = json.a("result");
        QString groupId = requiredData.data.s("wxid");
        qInfo() << "获取群" << groupId << "成员列表" << result.size() << "个";

        QMap<QString, QString> groupMemberNickMap;
        for (int i = 0; i < result.size(); i++)
        {
            MyJson item = result.at(i).toObject();
            QString wxid = item.s("wxid");
            QString groupNick = item.s("groupNick");
            groupMemberNickMap.insert(wxid, groupNick);
        }
        ac->setGroupMembers(groupId, groupMemberNickMap);

        // 更新群聊记录中的昵称
        ac->updateGroupChatsMemberNicks(groupId);
        
        // 更新View中显示的昵称
        emit st->signalGroupMemberListChanged(groupId);
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

        if (chatBean.isPrivate())
        {
            chatBean.senderName = ac->getFriend(chatBean.fromWxid).nick;
            chatBean.objectName = ac->getFriend(chatBean.fromWxid).nick;
            qInfo() << "收到私聊" << chatBean.objectName << chatBean.fromWxid << chatBean.msg.left(100);
        }
        else if (chatBean.isGroup())
        {
            chatBean.objectName = ac->getGroup(chatBean.fromWxid).nick;
            if (ac->getGroup(chatBean.fromWxid).groupMemberNickMap.contains(chatBean.finalFromWxid))
            {
                chatBean.senderName = ac->getGroup(chatBean.fromWxid).groupMemberNickMap[chatBean.finalFromWxid];
            }
            else
            {
                qWarning() << "获取群成员昵称失败，wxid:" << chatBean.finalFromWxid;
                QTimer::singleShot(100, this, [this, chatBean]() {
                    getGroupMemberList(chatBean.fromWxid);
                });
            }
            qInfo() << "收到群聊" << chatBean.objectName << chatBean.fromWxid << chatBean.senderName << chatBean.finalFromWxid << chatBean.msgType << chatBean.msg.left(100);
        }

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

void DeanService::getFriendList()
{
    qInfo() << "获取Hook微信好友列表";
    MyJson json;
    json.insert("type", "1");
    sendApiMessage("Q0005", json.toBa());
}

void DeanService::getGroupList()
{
    qInfo() << "获取Hook微信群列表";
    MyJson json;
    json.insert("type", "1");
    sendApiMessage("Q0006", json.toBa());
}

void DeanService::getGroupMemberList(QString groupId)
{
    qInfo() << "获取Hook微信群成员列表";
    MyJson json;
    json.insert("wxid", groupId);
    json.insert("type", "1");
    json.insert("getNick", "2");
    sendApiMessage("Q0008", json.toBa());
}

void DeanService::sendUserMessage(QString wxid, QString msg)
{
    MyJson json;
    json.insert("wxid", wxid);
    json.insert("msg", msg);
    sendApiMessage("sendText", json.toBa());
}
