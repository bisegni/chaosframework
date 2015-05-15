#ifndef HEALTWIDGETSLISTPRESENTEWIDGET_H
#define HEALTWIDGETSLISTPRESENTEWIDGET_H

#include "HealtPresenterWidget.h"

#include <QWidget>
#include <QSpacerItem>

namespace Ui {
class HealtWidgetsListPresenteWidget;
}

class HealtWidgetsListPresenteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HealtWidgetsListPresenteWidget(QWidget *parent = 0);
    ~HealtWidgetsListPresenteWidget();

    void addHealtWidget(HealtPresenterWidget *h_widget);
    void removeHealtWidget(HealtPresenterWidget *h_widget);

private:
    QSpacerItem *spacer;
    Ui::HealtWidgetsListPresenteWidget *ui;
};

#endif // HEALTWIDGETSLISTPRESENTEWIDGET_H
