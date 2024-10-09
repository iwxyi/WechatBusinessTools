#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "chatbean.h"

namespace Ui {
class ChatWidget;
}

enum ChatDirection {
    ChatOther = -1,
    ChatTips = 0,
    ChatSelf = 1
};

class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    // 最新消息列表
    void insertLatestMessageItem(int index, const ChatBean &chatBean);
    void removeLatestMessageItem(int index);
    void updateLatestMessageItem(int index,const ChatBean &chatBean);
    void updateAllMessagesByWxid(const QString &wxid);
    void updateAllMessages();
    void clearAll();

    // 聊天列表
    void loadMessages(const QString &wxid);
    void addMessage(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction);
    void addMessage(const ChatBean& chatBean);

private:
    QWidget* createMessageWidget(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction);

public slots:
    void onNewMessage(const ChatBean &chatBean);

private slots:
    void on_latestListWidget_currentRowChanged(int currentRow);

private:
    Ui::ChatWidget *ui;

    QStringList messageIdList;
    QString currentWxid;
};

#endif // CHATWIDGET_H
