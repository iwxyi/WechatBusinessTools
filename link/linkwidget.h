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

private slots:

    void on_deanServerUrlEdit_editingFinished();

    void on_deanWxidEdit_editingFinished();

private:
    Ui::LinkWidget *ui;
};

#endif // LINKWIDGET_H
