#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appruntime.h"
#include "signaltransfer.h"
#include "accountinfo.h"
#include "usettings.h"
#include "stringutil.h"
#include <QTableWidget>
#include <QCheckBox>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据服务
    deanService = new DeanService(this);
    ar->setDeanService((qint64)deanService);
    deanService->init();
    deanService->start();


    connect(st, &SignalTransfer::signalNickChanged, this, [=](QString nick) {
        this->setWindowTitle("已Hook：" + nick);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * 展示好友列表，每个列表有开关，默认关闭
 * 是黑名单还是白名单由设置决定
 */
void MainWindow::on_actionFriends_triggered()
{
    QWidget *widget = new QWidget(nullptr);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    // 创建表格
    QTableWidget *tableWidget = new QTableWidget(widget);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "wxid" << "好友" << "状态");
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSortingEnabled(true);

    // 加载表格
    auto loadFriendList = [=](QString filter) {
        // 清空表格
        tableWidget->clearContents();

        // 搜索词
        QList<FriendBean> friendList = ac->getFriendList();
        if (!filter.isEmpty()) {
            for (int i = 0; i < friendList.size(); i++) {
                FriendBean friendBean = friendList.at(i);
                if (!friendBean.nick.contains(filter) && !friendBean.wxid.contains(filter)) {
                    friendList.removeAt(i);
                    i--;
                }
            }
        }

        // 遍历好友列表
        qInfo() << "friendList:" << friendList.size();
        tableWidget->setRowCount(friendList.size());
        for (int i = 0; i < friendList.size(); i++) {
            FriendBean friendBean = friendList.at(i);
            QTableWidgetItem *item = new QTableWidgetItem(strLeft(friendBean.wxid, 30));
            item->setToolTip(friendBean.wxid);
            tableWidget->setItem(i, 0, item);

            item = new QTableWidgetItem(strLeft(friendBean.nick, 20));
            item->setToolTip(friendBean.nick);
            tableWidget->setItem(i, 1, item);

            QCheckBox *checkBox = new QCheckBox(tableWidget);
            checkBox->setChecked(us->isWhiteList_friendEnabled(friendBean.wxid));
            tableWidget->setCellWidget(i, 2, checkBox);
            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                us->setWhiteList_friendEnabled(friendBean.wxid, state == Qt::Checked);
                qInfo() << "friendBean.wxid:" << friendBean.wxid << "state:" << state;
            });
        }
    };
    loadFriendList("");

    // 自适应每列的宽度
    for (int i = 0; i < tableWidget->columnCount(); i++) {
        tableWidget->resizeColumnToContents(i);
    }

    // 搜索输入框
    QLineEdit *searchLineEdit = new QLineEdit(widget);
    searchLineEdit->setPlaceholderText("搜索好友");
    layout->addWidget(searchLineEdit);
    connect(searchLineEdit, &QLineEdit::textChanged, this, [=](const QString &text) {
        qInfo() << "searchLineEdit:" << text;
        loadFriendList(text);
    });

    // 添加表格到布局
    layout->addWidget(tableWidget);
    layout->addWidget(searchLineEdit);

    // 自适应宽度
    int totalWidth = 100;
    for (int i = 0; i < tableWidget->columnCount(); i++) {
        totalWidth += tableWidget->columnWidth(i);
    }
    widget->setMinimumWidth(totalWidth);
    widget->setMinimumHeight(totalWidth);
    widget->show();
}


void MainWindow::on_actionGroups_triggered()
{
    QWidget *widget = new QWidget(nullptr);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    // 创建表格
    QTableWidget *tableWidget = new QTableWidget(widget);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "wxid" << "群" << "状态");
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSortingEnabled(true);

    // 加载表格
    auto loadGroupList = [=](QString filter) {
        // 清空表格
        tableWidget->clearContents();

        // 搜索词
        QList<GroupBean> groupList = ac->getGroupList();
        if (!filter.isEmpty()) {
            for (int i = 0; i < groupList.size(); i++) {
                GroupBean groupBean = groupList.at(i);
                if (!groupBean.nick.contains(filter) && !groupBean.wxid.contains(filter)) {
                    groupList.removeAt(i);
                    i--;
                }
            }
        }

        // 遍历群组列表
        qInfo() << "groupList:" << groupList.size();
        tableWidget->setRowCount(groupList.size());
        for (int i = 0; i < groupList.size(); i++) {
            GroupBean groupBean = groupList.at(i);
            QTableWidgetItem *item = new QTableWidgetItem(strLeft(groupBean.wxid, 30));
            item->setToolTip(groupBean.wxid);
            tableWidget->setItem(i, 0, item);

            item = new QTableWidgetItem(strLeft(groupBean.nick, 20));
            item->setToolTip(groupBean.nick);
            tableWidget->setItem(i, 1, item);

            QCheckBox *checkBox = new QCheckBox(tableWidget);
            checkBox->setChecked(us->isWhiteList_groupEnabled(groupBean.wxid));
            tableWidget->setCellWidget(i, 2, checkBox);
            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                us->setWhiteList_groupEnabled(groupBean.wxid, state == Qt::Checked);
                qInfo() << "groupBean.wxid:" << groupBean.wxid << "state:" << state;
            });
        }
    };
    loadGroupList("");

    // 自适应每列的宽度
    for (int i = 0; i < tableWidget->columnCount(); i++) {
        tableWidget->resizeColumnToContents(i);
    }

    // 搜索输入框
    QLineEdit *searchLineEdit = new QLineEdit(widget);
    searchLineEdit->setPlaceholderText("搜索群");
    layout->addWidget(searchLineEdit);

    // 添加表格到布局
    layout->addWidget(tableWidget);
    layout->addWidget(searchLineEdit);

    // 自适应宽度
    int totalWidth = 100;
    for (int i = 0; i < tableWidget->columnCount(); i++) {
        totalWidth += tableWidget->columnWidth(i);
    }
    widget->setMinimumWidth(totalWidth);
    widget->setMinimumHeight(totalWidth);
    widget->show();
}

