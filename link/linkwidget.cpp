#include "linkwidget.h"
#include "ui_linkwidget.h"
#include "usettings.h"
#include "appruntime.h"
#include "deanservice.h"
#include "signaltransfer.h"

LinkWidget::LinkWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LinkWidget)
{
    ui->setupUi(this);
    this->setObjectName("LinkWidget");

    // 设置数据
    ui->deanServerUrlEdit->setText(us->deanServerUrl());
    ui->deanWxidEdit->setText(us->deanWxid());

    // 连接信号
    connect(st, &SignalTransfer::signalWxidChanged, this, [=](QString wxid) {
        ui->deanWxidEdit->setText(wxid);
    });
}

LinkWidget::~LinkWidget()
{
    delete ui;
}

void LinkWidget::on_deanServerUrlEdit_editingFinished()
{
    us->setDeanWxid(ui->deanWxidEdit->text());;
    emit st->signalReconnectWs();
}


void LinkWidget::on_deanWxidEdit_editingFinished()
{
    us->setDeanServerUrl(ui->deanServerUrlEdit->text());
    us->setValue("us/deanServerUrl", ui->deanServerUrlEdit->text());
    emit st->signalReconnectWs();
}

