#ifndef CHATBEAN_H
#define CHATBEAN_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

enum ChatType
{
    ChatType_Private,
    ChatType_Group,
    ChatType_Public,
    ChatType_System
};

enum ChatMsgType
{
    ChatMsgType_Text,
    ChatMsgType_Image,
    ChatMsgType_Voice,
    ChatMsgType_Card,
    ChatMsgType_Video,
    ChatMsgType_Emotion,
    ChatMsgType_Location,
    ChatMsgType_Link,
    ChatMsgType_RedPacket,
    ChatMsgType_MiniProgram,
    ChatMsgType_GroupInvite,
    ChatMsgType_System
};

struct ChatBean
{
    QString des;
    qint64 timeStamp; // 收到这条消息的13位现行时间戳
    int fromType; // 来源类型：1|私聊 2|群聊 3|公众号
    int msgType; // 消息类型：1|文本 3|图片 34|语音 42|名片 43|视频 47|动态表情 48|地理位置 49|分享链接或附件 2001|红包 2002|小程序 2003|群邀请 10000|系统消息
    int msgSource; // 消息来源：0|自己 1|别人
    QString fromWxid; // fromType=1时为好友wxid，fromType=2时为群wxid，fromType=3时公众号wxid
    QString finalFromWxid; // 仅fromType=2时有效，为群内发言人wxid
    QStringList atWxidList; // 仅fromType=2时有效，为消息中艾特人wxid列表
    int silence; // 仅fromType=2时有效，0
    int membercount; // 仅fromType=2时有效，群成员数量
    QString signature; // 消息签名
    QString msg; // 消息内容
    QString msgId; // 消息ID
    QString msgBase64; // 消息内容的Base64

    QString objectName; // 私聊发送者名称/群聊名称
    QString senderName; // 消息发送者昵称

    QString getObjectId() const
    {
        return fromWxid;
    }

    QString getSenderId() const
    {
        return finalFromWxid;
    }

    QString getMsg() const
    {
        if (msgType == 2)
        {
            return "[表情包]";
        }
        else if (msgType == 3)
        {
            return "[图片]";
        }
        else if (msgType == 34)
        {
            return "[语音]";
        }
        else if (msgType == 42)
        {
            return "[名片]";
        }
        else if (msgType == 43)
        {
            return "[视频]";
        }
        else if (msgType == 47)
        {
            return "[动态表情]";
        }
        else if (msgType == 48)
        {
            return "[地理位置]";
        }
        else if (msgType == 49)
        {
            return "[分享链接]";
        }
        else if (msgType == 2001)
        {
            return "[红包]";
        }
        else if (msgType == 2002)
        {
            return "[小程序]";
        }
        else if (msgType == 2003)
        {
            return "[群邀请]";
        }
        else if (msgType == 10000)
        {
            return "[系统消息]";
        }
        if (msg.startsWith("<msg>") && msg.endsWith("</msg>"))
        {
            return "[xml]";
        }
        return msg;
    }

    bool isPrivate() const
    {
        return fromType == 1;
    }

    bool isGroup() const
    {
        return fromType == 2;
    }

    bool isPublic() const
    {
        return fromType == 3;
    }

    bool isSystem() const
    {
        return fromType == 10000;
    }

    bool isMe() const
    {
        return msgSource == 0;
    }

    bool isOther() const
    {
        return msgSource == 1;
    }
};

struct FriendBean
{
    QString wxid;
    QString wxNum;
    QString nick;
    QString remark;
    QString nickBrief;
    QString nickWhole;
    QString remarkBrief;
    QString remarkWhole;
    QString enBrief;
    QString enWhole;
    QString v3;
    QString sign;
    QString country;
    QString province;
    QString city;
    QString momentsBackgroudImgUrl;
    QString avatarMinUrl;
    QString avatarMaxUrl;
    QString sex;

    QString getName() const
    {
        if (!remark.isEmpty())
        {
            return remark;
        }
        else if (!nick.isEmpty())
        {
            return nick;
        }
        else if (!wxNum.isEmpty())
        {
            return wxNum;
        }
        return wxid;
    }
};

struct GroupBean : public FriendBean
{
    int groupMemberNum;
    QString groupManger;

    QMap<QString, QString> groupMemberNickMap;
};

#endif // CHATBEAN_H
