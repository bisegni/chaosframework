#ifndef LOGENTRYTABLEMODEL_H
#define LOGENTRYTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class LogEntryTableModel:
public ChaosAbstractTableModel,
public ApiHandler {
public:
    LogEntryTableModel(QObject *parent = 0);

    //!update log entries for node uid as emitter and log domain list to inclue
    void updateEntries(const QString& node_uid,
                       const chaos::metadata_service_client::api_proxy::logging::LogDomainList& domain_list);
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::logging::LogEntry> getLogEntryForRow(unsigned int row);
    void clear();
    void nextPage();
    void previousPage();
    void setPageLength(uint32_t _page_length);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCellUserData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;

    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private:
    ApiSubmitter api_submitter;
    uint32_t page_lenght;
    QString node_uid;
    chaos::metadata_service_client::api_proxy::logging::LogDomainList domain_list;
    uint64_t current_page_sequence;
    uint64_t last_received_sequence_id;
    std::auto_ptr<chaos::metadata_service_client::api_proxy::logging::GetLogForSourceUIDHelper> helper;
};

#endif // LOGENTRYTABLEMODEL_H
