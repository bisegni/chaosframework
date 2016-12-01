#ifndef SNAPSHOTDATASETNODELISTMODEL_H
#define SNAPSHOTDATASETNODELISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <QVector>
#include <QSharedPointer>

class SnapshotDatasetNodeListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {
public:
    SnapshotDatasetNodeListModel(QObject *parent = 0);
    ~SnapshotDatasetNodeListModel();
    //update the list with all the domain emitted by source uid
    void updateDatasetListFor(const QString& node_name,
                              const QString& snapshot_name);
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    bool setRowCheckState(const int row, const QVariant& value);
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private:
    ApiSubmitter api_submitter;
    chaos::common::data::VectorStrCDWShrdPtr dataset_in_snapshot;
};

#endif // SNAPSHOTDATASETNODELISTMODEL_H
