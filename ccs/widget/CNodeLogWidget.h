#ifndef CNODELOGWIDGET_H
#define CNODELOGWIDGET_H

#include "ChaosWidgetCompanion.h"
#include "../data/LogDomainListModel.h"
#include "../data/LogEntryTableModel.h"
#include "../data/LogDataTableModel.h"

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

    qint32 p_page_length;
    Q_PROPERTY(qint32 page_length READ pageLength WRITE setPageLength)

public:
    explicit CNodeLogWidget(QWidget *parent = 0);
    ~CNodeLogWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();

    void setPageLength(qint32 page_length);
    qint32 pageLength();
protected:
    void handleLogEvent(const std::string source,
                        const std::string domain);
private slots:
    void logTypesDataChanged(const QModelIndex& top_left,
                             const QModelIndex& bottom_right,
                             const QVector<int>& roles);

    void logEntriesTableSelectionChanged(const QModelIndex& current_selection,
                                         const QModelIndex& previous_selection);
    void on_pushButtonUpdateLogTypes_clicked();

    void on_pushButtonLogEntriesNextPage_clicked();

    void on_pushButtonLogEntriesPreviousPage_clicked();

private:
    Ui::CNodeLogWidget *ui;
    LogDomainListModel  domain_list_model;
    LogEntryTableModel  entry_table_model;
    LogDataTableModel   data_table_model;
};

#endif // CNODELOGWIDGET_H
