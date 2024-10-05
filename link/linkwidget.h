#ifndef LINKWIDGET_H
#define LINKWIDGET_H

#include <QWidget>

namespace Ui {
class LinkWidget;
}

class LinkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LinkWidget(QWidget *parent = nullptr);
    ~LinkWidget();

public slots:
    void onDeanServerUrlEditFinished();
    void onDeanWxidEditFinished();

private:
    Ui::LinkWidget *ui;
};

#endif // LINKWIDGET_H
