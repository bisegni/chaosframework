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
    explicit CPopupWidgetContainer(QWidget *parent);
    ~CPopupWidgetContainer();
    void addWidget(QWidget *custom_widget);
private slots:
    void on_pushButtonCancel_clicked();

private:
    Ui::CPopupWidgetContainer *ui;
};

#endif // CPOPUPWIDGETCONTAINER_H
