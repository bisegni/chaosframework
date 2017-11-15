#ifndef CPOPUPWIDGETCONTAINER_H
#define CPOPUPWIDGETCONTAINER_H

#include <QWidget>

namespace Ui {
class CPopupWidgetContainer;
}

class CPopupWidgetContainer : public QWidget
{
    Q_OBJECT

public:
    explicit CPopupWidgetContainer(QWidget *custom_widget, QWidget *parent = 0);
    ~CPopupWidgetContainer();

private slots:
    void on_pushButtonCancel_clicked();

private:
    Ui::CPopupWidgetContainer *ui;
};

#endif // CPOPUPWIDGETCONTAINER_H
