#include "linkwidget.h"
#include "ui_linkwidget.h"

LinkWidget::LinkWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LinkWidget)
{
    ui->setupUi(this);
}

LinkWidget::~LinkWidget()
{
    delete ui;
}
