#ifndef CNODELOGWIDGET_H
#define CNODELOGWIDGET_H

#include "ChaosWidgetCompanion.h"
#include "../data/LogDomainListModel.h"

#include <QWidget>

namespace Ui {
class CNodeLogWidget;
}

class CNodeLogWidget :
        public QWidget,
        public ChaosWidgetCompanion {
    Q_OBJECT

public:
    explicit CNodeLogWidget(QWidget *parent = 0);
    ~CNodeLogWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
private:
    Ui::CNodeLogWidget *ui;
    LogDomainListModel domain_list_model;
};

#endif // CNODELOGWIDGET_H
