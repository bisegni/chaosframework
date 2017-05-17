#ifndef SNAPSHOTTABLEMODEL_H
#define SNAPSHOTTABLEMODEL_H

#include "../data/ChaosAbstractTableModel.h"
#include <ChaosMetadataServiceClient/api_proxy/api.h>

#include <QSharedPointer>

class SnapshotTableModel:
        public ChaosAbstractTableModel {
    Q_OBJECT
public:
    SnapshotTableModel(QObject *parent = 0);
    ~SnapshotTableModel();
    void updateSnapshotList(ChaosUniquePtr<chaos::metadata_service_client::api_proxy::service::GetAllSnapshotHelper> _snapshot_info_list);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getBackgroundForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;
private:
    ChaosUniquePtr<chaos::metadata_service_client::api_proxy::service::GetAllSnapshotHelper> snapshot_info_list;
};

#endif // SNAPSHOTTABLEMODEL_H
