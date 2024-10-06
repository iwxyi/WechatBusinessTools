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
    void insertLatestMessageItem(const ChatBean &chatBean);
    void removeLatestMessageItem(int index);
    void updateLatestMessageItem(int index,const ChatBean &chatBean);

    // 聊天列表
    void addMessage(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction);

private:
    QWidget* createMessageWidget(const QString &nickname, const QString &wxid, const QString &message, ChatDirection direction);

public slots:
    void onNewMessage(const ChatBean &chatBean);

private:
    Ui::ChatWidget *ui;
    QVBoxLayout *chatLayout;

    QStringList messageIdList;
    QString currentWxid;
};

#endif // CHATWIDGET_H
