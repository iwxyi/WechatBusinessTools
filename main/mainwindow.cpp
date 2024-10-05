#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appruntime.h"
#include "signaltransfer.h"

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

