#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据服务
    deanService = new DeanService(this);
    deanService->init();
    deanService->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

