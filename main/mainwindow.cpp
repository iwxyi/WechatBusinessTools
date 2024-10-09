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
#include <QPushButton>

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
            checkBox->setChecked(us->isWhiteList_friendChecked(friendBean.wxid));
            tableWidget->setCellWidget(i, 2, checkBox);
            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                us->setWhiteList_friendChecked(friendBean.wxid, state == Qt::Checked);
                qDebug() << "调整friendBean.wxid:" << friendBean.wxid << "state:" << state;
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

    // 全选/反选按钮
    QPushButton *selectAllButton = new QPushButton("全选", widget);
    QPushButton *selectNoneButton = new QPushButton("反选", widget);
    connect(selectAllButton, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            QCheckBox* checkBox = (QCheckBox*)tableWidget->cellWidget(i, 2);
            checkBox->setCheckState(Qt::Checked);
        }
    });
    connect(selectNoneButton, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            QCheckBox* checkBox = (QCheckBox*)tableWidget->cellWidget(i, 2);
            checkBox->setCheckState(checkBox->checkState() ? Qt::Unchecked : Qt::Checked);
        }
    });

    // 黑名单模式
    QCheckBox *blackModeCheckBox = new QCheckBox("白名单模式", widget);
    blackModeCheckBox->setToolTip("白名单/黑名单模式\n白名单：只显示选中的好友\n黑名单：显示未选中的好友\n新好友默认不会被选中");
    blackModeCheckBox->setChecked(us->useWhiteOrBlackList_friend());
    connect(blackModeCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        us->setUseWhiteOrBlackList_friend(state == Qt::Checked);
    });

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(selectNoneButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(blackModeCheckBox);

    // 添加表格到布局
    layout->addWidget(tableWidget);
    layout->addWidget(searchLineEdit);
    layout->addLayout(buttonLayout);

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
            checkBox->setChecked(us->isWhiteList_groupChecked(groupBean.wxid));
            tableWidget->setCellWidget(i, 2, checkBox);
            connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
                us->setWhiteList_groupChecked(groupBean.wxid, state == Qt::Checked);
                qDebug() << "调整groupBean.wxid:" << groupBean.wxid << "state:" << state;
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
    connect(searchLineEdit, &QLineEdit::textChanged, this, [=](const QString &text) {
        qInfo() << "searchLineEdit:" << text;
        loadGroupList(text);
    });

    // 全选/反选按钮
    QPushButton *selectAllButton = new QPushButton("全选", widget);
    QPushButton *selectNoneButton = new QPushButton("反选", widget);
    connect(selectAllButton, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            QCheckBox* checkBox = (QCheckBox*)tableWidget->cellWidget(i, 2);
            checkBox->setCheckState(Qt::Checked);
        }
    });
    connect(selectNoneButton, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            QCheckBox* checkBox = (QCheckBox*)tableWidget->cellWidget(i, 2);
            checkBox->setCheckState(checkBox->checkState() ? Qt::Unchecked : Qt::Checked);
        }
    });

    // 黑名单模式
    QCheckBox *blackModeCheckBox = new QCheckBox("白名单模式", widget);
    blackModeCheckBox->setToolTip("白名单/黑名单模式\n白名单：只显示选中的群\n黑名单：显示未选中的群\n新群组默认不会被选中");
    blackModeCheckBox->setChecked(us->useWhiteOrBlackList_group());
    connect(blackModeCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        us->setUseWhiteOrBlackList_group(state == Qt::Checked);
    });
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(selectNoneButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(blackModeCheckBox);

    // 添加到布局
    layout->addWidget(tableWidget);
    layout->addWidget(searchLineEdit);
    layout->addLayout(buttonLayout);

    // 自适应宽度
    int totalWidth = 100;
    for (int i = 0; i < tableWidget->columnCount(); i++) {
        totalWidth += tableWidget->columnWidth(i);
    }
    widget->setMinimumWidth(totalWidth);
    widget->setMinimumHeight(totalWidth);
    widget->show();
}

