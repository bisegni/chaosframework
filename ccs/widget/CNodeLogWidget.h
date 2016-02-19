#ifndef CNODELOGWIDGET_H
#define CNODELOGWIDGET_H

#include "ChaosWidgetCompanion.h"
#include "../data/LogDomainListModel.h"
#include "../data/LogEntryTableModel.h"

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
private slots:
    void logTypesDataChanged(const QModelIndex& top_left,
                             const QModelIndex& bottom_right,
                             const QVector<int>& roles);
private:
    Ui::CNodeLogWidget *ui;
    LogDomainListModel domain_list_model;
    LogEntryTableModel entry_table_model;
};

#endif // CNODELOGWIDGET_H
