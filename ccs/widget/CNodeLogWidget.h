#ifndef CNODELOGWIDGET_H
#define CNODELOGWIDGET_H

#include "ChaosWidgetCompanion.h"
#include "../data/LogDomainListModel.h"
#include "../data/LogEntryTableModel.h"
#include "../data/LogDataTableModel.h"

#include <QTimer>
#include <QWidget>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class CNodeLogWidget;
}

class CNodeLogWidget :
        public QWidget,
        public ChaosWidgetCompanion,
        protected chaos::metadata_service_client::event::alert::AlertLogEventHandler {
    Q_OBJECT

    qint32 p_max_result_item;
    Q_PROPERTY(qint32 page_length READ maxResultItem WRITE setMaxResultItem)

    void updateEntryList();
public:
    explicit CNodeLogWidget(QWidget *parent = 0);
    ~CNodeLogWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();

    void setMaxResultItem(qint32 max_result_item);
    qint32 maxResultItem();
protected:
    void handleLogEvent(const std::string source,
                        const std::string domain);
private slots:
    void timeoutUpdateTimer();
    void logTypesDataChanged(const QModelIndex& top_left,
                             const QModelIndex& bottom_right,
                             const QVector<int>& roles);

    void logEntriesTableSelectionChanged(const QModelIndex& current_selection,
                                         const QModelIndex& previous_selection);
    void on_pushButtonUpdateLogTypes_clicked();

    void on_lineEditMaxNumberOfResult_returnPressed();

    void on_lineEditRefreshUpdate_editingFinished();

private:
    QTimer              refresh_timer;
    Ui::CNodeLogWidget *ui;
    LogDomainListModel  domain_list_model;
    LogEntryTableModel  entry_table_model;
    LogDataTableModel   data_table_model;
};

#endif // CNODELOGWIDGET_H
