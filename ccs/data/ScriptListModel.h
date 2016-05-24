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
private:
    ApiSubmitter api_submitter;
};

#endif // SCRIPTLISTMODEL_H
