#ifndef SCRIPTLISTMODEL_H
#define SCRIPTLISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

class ScriptListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    bool setRowCheckState(const int row, const QVariant& value);
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    ScriptListModel(QObject *parent=0);
    void updateSearchString(const QString& new_search_string);
    void updateSearch();
private:
    QString search_string;
    uint64_t last_sequence_id;
    chaos::service_common::data::script::ScriptBaseDescriptionListWrapper script_base_list_wrapper;
    ApiSubmitter api_submitter;
};

#endif // SCRIPTLISTMODEL_H
