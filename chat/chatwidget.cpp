#include "chatwidget.h"
#include "ui_chatwidget.h"
#include "signaltransfer.h"
#include "accountinfo.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    chatLayout = new QVBoxLayout(ui->msgListWidget);
    chatLayout->setAlignment(Qt::AlignTop);
    ui->msgListWidget->setLayout(chatLayout);

    connect(st, &SignalTransfer::signalNewMessage, this, &ChatWidget::onNewMessage);
    connect(st, &SignalTransfer::signalGroupMemberListChanged, this, [=](QString groupId) {
        qInfo() << "更新View群成员信息" << groupId;
        for (int i = 0; i < messageIdList.size(); i++)
        {
            if (messageIdList[i] == groupId)
            {
                updateLatestMessageItem(i, ac->getLatestChat(groupId));
                return;
            }
        }
        qWarning() << "更新View群成员信息失败，未找到聊天记录中的群组，groupId:" << groupId;
    });
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::onNewMessage(const ChatBean &chatBean)
{
    QString wxid = chatBean.getObjectId();
    if (!messageIdList.contains(wxid)) // 如果不在列表中，则插入到最前面
    {
        // qInfo() << "插入最新消息列表，wxid:" << wxid << ", 消息:" << chatBean.msg.left(100);
        insertLatestMessageItem(chatBean);
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
        insertLatestMessageItem(chatBean);
    }
}

/**
 * 插入最新消息到latestListWidget
 */
void ChatWidget::insertLatestMessageItem(const ChatBean &chatBean)
{
    messageIdList.insert(0, chatBean.getObjectId());

    QString firstMsgLine = chatBean.getMsg().split("\n").first();
    
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    QLabel *nicknameLabel = new QLabel(chatBean.objectName, widget);
    QLabel *messageLabel = new QLabel(chatBean.senderName + "：" + firstMsgLine, widget);
    nicknameLabel->setObjectName("nicknameLabel");
    messageLabel->setObjectName("messageLabel");
    layout->addWidget(nicknameLabel);
    layout->addWidget(messageLabel);
    widget->setLayout(layout);

    QListWidgetItem *item = new QListWidgetItem();
    ui->latestListWidget->insertItem(0, item);
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

    QString firstMsgLine = chatBean.msg.split("\n").first();

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
 * 添加消息到聊天列表
 */
void ChatWidget::addMessage(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction)
{
    QWidget *messageWidget = createMessageWidget(nickname, wxid, message, direction);
    chatLayout->addWidget(messageWidget);
}

QWidget* ChatWidget::createMessageWidget(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    if (direction == ChatTips) {
        QLabel *tipsLabel = new QLabel(message, widget);
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
