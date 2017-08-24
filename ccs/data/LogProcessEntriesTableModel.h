#ifndef LOGPROCESSENTRIESTABLEMODEL_H
#define LOGPROCESSENTRIESTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include <string>
#include <vector>

class LogProcessEntriesTableModel:
        public ChaosAbstractTableModel,
        public ApiHandler {
public:
    LogProcessEntriesTableModel(const QString& _node_uid,
                         QObject *parent = 0);
    void startSearchEntry();
    void next();
    void prev();
    virtual void clear();
    void setMaxResultItem(uint32_t _number_of_max_result);
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
protected:
    const QString node_uid;
    uint64_t first_seq;
    uint64_t last_seq;
    uint32_t number_of_max_result;
    ApiSubmitter api_submitter;
    chaos::service_common::data::agent::VectorAgentLogEntry found_entires;
};
#endif // LOGPROCESSENTRIESTABLEMODEL_H
