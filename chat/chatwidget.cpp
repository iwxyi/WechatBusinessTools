#include "chatwidget.h"
#include "ui_chatwidget.h"
#include "signaltransfer.h"
#include "accountinfo.h"
#include "usettings.h"
#include "debounce.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);

    connect(st, &SignalTransfer::signalNewMessage, this, &ChatWidget::onNewMessage);
    connect(st, &SignalTransfer::signalGroupMemberListChanged, this, &ChatWidget::updateAllMessagesByWxid);

    connect(us, &USettings::signalFriendEnabledChanged, this, [&](QString wxid, bool enabled) {
        debounce->call("CHAT"_shash, this, [=]{
            updateAllMessages();
        });
    });
    connect(us, &USettings::signalGroupEnabledChanged, this, [&](QString wxid, bool enabled) {
        debounce->call("CHAT"_shash, this, [=]{
            updateAllMessages();
        });
    });
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::onNewMessage(const ChatBean &chatBean)
{
    // 判断是否是忽略的
    if (chatBean.isPrivate() && !us->isFriendEnabled(chatBean.getObjectId()))
    {
        qDebug() << "跳过忽略好友消息，wxid:" << chatBean.getObjectId() << chatBean.objectName;
        return;
    }
    if (chatBean.isGroup() && !us->isGroupEnabled(chatBean.getObjectId()))
    {
        qDebug() << "跳过忽略群消息，groupId:" << chatBean.getObjectId() << chatBean.objectName;
        return;
    }

    // 添加消息
    QString wxid = chatBean.getObjectId();
    if (!messageIdList.contains(wxid)) // 如果不在列表中，则插入到最前面
    {
        // qInfo() << "插入最新消息列表，wxid:" << wxid << ", 消息:" << chatBean.msg.left(100);
        insertLatestMessageItem(0, chatBean);
        return;
    }
    
    // 更新最新列表的消息
    int index = messageIdList.indexOf(wxid);
    Q_ASSERT(index != -1);
    if (index == 0) // 已经是最新的了，那就不更换顺序，直接更新
    {
        // qInfo() << "更新最新消息列表 索引:" + QString::number(index) + "，wxid:" << wxid << ", 消息:" << chatBean.msg.left(100);
        updateLatestMessageItem(index, chatBean);
    }
    else // 不是最新的，切换到最顶上
    {
        // qInfo() << "移动索引" << index << "至最顶上，wxid:" << wxid << ", 消息:" << chatBean.msg.left(100);
        removeLatestMessageItem(index);
        insertLatestMessageItem(0, chatBean);
    }

    // 当前打开的消息记录
    if (currentWxid == wxid)
    {
        addMessage(chatBean);
        ui->msgListWidget->scrollToBottom();
    }
}

/**
 * 插入最新消息到latestListWidget
 */
void ChatWidget::insertLatestMessageItem(int index, const ChatBean &chatBean)
{
    if (index == -1)
        index = messageIdList.size();
    messageIdList.insert(index, chatBean.getObjectId());

    QString firstMsgLine = chatBean.getMsg().split("\n").first();
    
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    QLabel *nicknameLabel = new QLabel(chatBean.objectName, widget);
    QLabel *messageLabel = new QLabel(chatBean.senderName + "：" + firstMsgLine, widget);
    nicknameLabel->setObjectName("nicknameLabel");
    messageLabel->setObjectName("messageLabel");
    nicknameLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(nicknameLabel);
    layout->addWidget(messageLabel);
    widget->setLayout(layout);

    // 设置昵称标签字体加粗
    QFont boldFont = nicknameLabel->font();
    boldFont.setBold(true);
    nicknameLabel->setFont(boldFont);
    // 设置消息标签字体颜色为灰色
    QPalette palette = messageLabel->palette();
    palette.setColor(QPalette::WindowText, Qt::gray);
    messageLabel->setPalette(palette);

    QListWidgetItem *item = new QListWidgetItem();
    ui->latestListWidget->insertItem(index, item);
    ui->latestListWidget->setItemWidget(item, widget);
    widget->adjustSize();
    item->setSizeHint(QSize(0, widget->height()));
}

/**
 * 更新最新消息列表
 */
void ChatWidget::updateLatestMessageItem(int index, const ChatBean &chatBean)
{
    messageIdList[index] = chatBean.getObjectId();

    QString firstMsgLine = chatBean.getMsg().split("\n").first();

    QListWidgetItem *item = ui->latestListWidget->item(index);
    Q_ASSERT(item != nullptr);
    QWidget *widget = ui->latestListWidget->itemWidget(item);
    Q_ASSERT(widget != nullptr);

    QLabel *nicknameLabel = widget->findChild<QLabel*>("nicknameLabel");
    Q_ASSERT(nicknameLabel != nullptr);
    nicknameLabel->setText(chatBean.objectName);

    QLabel *messageLabel = widget->findChild<QLabel*>("messageLabel");
    Q_ASSERT(messageLabel != nullptr);
    messageLabel->setText(chatBean.senderName + "：" + firstMsgLine);
}

/**
 * 删除最新消息列表
 */
void ChatWidget::removeLatestMessageItem(int index)
{
    messageIdList.removeAt(index);

    QListWidgetItem *item = ui->latestListWidget->item(index);
    Q_ASSERT(item != nullptr);
    QWidget *widget = ui->latestListWidget->itemWidget(item);
    Q_ASSERT(widget != nullptr);
    ui->latestListWidget->removeItemWidget(item);
    delete item;
    delete widget;
}

/**
 * 群成员昵称更新时，同步更新所有界面消息文字
 */
void ChatWidget::updateAllMessagesByWxid(const QString &wxid)
{
    qInfo() << "更新View对象成员信息" << wxid;
    bool isUpdated = false;
    for (int i = 0; i < messageIdList.size(); i++)
    {
        if (messageIdList[i] == wxid)
        {
            updateLatestMessageItem(i, ac->getLatestChat(wxid));
            isUpdated = true;
            break;
        }
    }
    if (!isUpdated)
    {
        qWarning() << "更新View群成员信息失败，未找到聊天记录中的群组，groupId:" << wxid;
    }

    // 如果已经打开的是这个消息列表，则更新
    if (currentWxid == wxid)
    {
        loadMessages(wxid);
    }
}

void ChatWidget::updateAllMessages()
{
    // 全部清空
    clearAll();

    // 加载最近消息列表
    qInfo() << "更新所有消息列表";
    QList<ChatBean> allChats = ac->getAllLatestChats();
    for (const ChatBean& chatBean : allChats)
    {
        if (chatBean.isPrivate())
        {
            if (us->isFriendEnabled(chatBean.getObjectId()))
            {
                qDebug() << "显示好友消息，wxid:" << chatBean.getObjectId() << chatBean.objectName;
                insertLatestMessageItem(-1, chatBean);
            }
            else
            {
                qDebug() << "跳过好友消息，wxid:" << chatBean.getObjectId() << chatBean.objectName;
            }
        }
        else if (chatBean.isGroup())
        {
            if (us->isGroupEnabled(chatBean.getObjectId()))
            {
                qDebug() << "显示群消息，groupId:" << chatBean.getObjectId() << chatBean.objectName;
                insertLatestMessageItem(-1, chatBean);
            }
            else
            {
                qDebug() << "跳过群消息，groupId:" << chatBean.getObjectId() << chatBean.objectName;
            }
        }
    }
}

void ChatWidget::clearAll()
{
    qInfo() << "清空所有消息列表";
    messageIdList.clear();
    ui->latestListWidget->clear();
    loadMessages("");
}

/**
 * 右边的聊天列表
 */
void ChatWidget::loadMessages(const QString &wxid)
{
    // 清空旧的聊天列表
    QList<QWidget*> widgets;
    for (int i = 0; i < ui->msgListWidget->count(); ++i) {
    QWidget* widget = ui->msgListWidget->itemWidget(ui->msgListWidget->item(i));
    if (widget) {
        widget->deleteLater();
    } else {
        qWarning() << "未找到要删除的对话控件";
    }
    }
    ui->msgListWidget->clear();

    // 加载消息
    if (wxid.isEmpty())
    {
        qWarning() << "跳过加载消息列表，wxid为空";
        return;
    }
    currentWxid = wxid;
    QList<ChatBean> chatBeanList = ac->getChatList(wxid);
    qInfo() << "加载消息列表，wxid:" << wxid << ", 消息数量:" << chatBeanList.size();
    
    // 加载最多20条消息
    int maxCount = us->maxMessageCount();
    int startIndex = chatBeanList.size() - maxCount;
    if (startIndex < 0)
        startIndex = 0;
    for (int i = startIndex; i < chatBeanList.size(); i++)
    {
        addMessage(chatBeanList[i]);
    }

    // 滚动到底部
    ui->msgListWidget->scrollToBottom();
}

/**
 * 添加消息到聊天列表
 */
void ChatWidget::addMessage(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction)
{
    QWidget *messageWidget = createMessageWidget(nickname, wxid, message, direction);
    QListWidgetItem *item = new QListWidgetItem(ui->msgListWidget);
    ui->msgListWidget->setItemWidget(item, messageWidget);
    item->setSizeHint(QSize(0, messageWidget->height()));
}

void ChatWidget::addMessage(const ChatBean &chatBean)
{
    ChatDirection direction = ChatDirection::ChatTips;
    if (chatBean.finalFromWxid == ac->getWxid())
        direction = ChatDirection::ChatSelf;
    else
        direction = ChatDirection::ChatOther;
    addMessage(chatBean.senderName, chatBean.finalFromWxid, chatBean.getMsg(), direction);
}

QWidget* ChatWidget::createMessageWidget(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    if (direction == ChatTips) {
        QLabel *tipsLabel = new QLabel(message, widget);
        tipsLabel->setObjectName("tipsLabel");
        tipsLabel->setAlignment(Qt::AlignCenter);
        tipsLabel->setStyleSheet("color: gray; font-style: italic;");
        layout->addWidget(tipsLabel);
    } else {
        QLabel *nicknameLabel = new QLabel(nickname, widget);
        QLabel *messageLabel = new QLabel(message, widget);

        nicknameLabel->setStyleSheet("font-weight: bold;");
        messageLabel->setWordWrap(true);

        if (direction == ChatSelf) {
            layout->addStretch();
            layout->addWidget(nicknameLabel);
            layout->addWidget(messageLabel);
        } else {
            layout->addWidget(nicknameLabel);
            layout->addWidget(messageLabel);
            layout->addStretch();
        }
    }

    widget->setLayout(layout);
    return widget;
}

/**
 * 切换最新消息列表
 */
void ChatWidget::on_latestListWidget_currentRowChanged(int currentRow)
{
    if (currentRow == -1)
    {
        loadMessages("");
        return;
    }
    qInfo() << "切换最新消息列表，currentRow:" << currentRow;
    QString wxid = messageIdList[currentRow];
    loadMessages(wxid);
}

