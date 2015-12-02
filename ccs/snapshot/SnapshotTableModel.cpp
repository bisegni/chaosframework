#include "SnapshotTableModel.h"

SnapshotTableModel::SnapshotTableModel(QObject *parent):
    ChaosAbstractTableModel(parent) {

}

SnapshotTableModel::~SnapshotTableModel() {

}

void SnapshotTableModel::updateSnapshotList(QSharedPointer<chaos::metadata_service_client::api_proxy::service::GetAllSnapshotHelper> _snapshot_info_list) {
 beginResetModel();
 snapshot_info_list = _snapshot_info_list;
 endResetModel();
}

int SnapshotTableModel::getRowCount() const {
    return snapshot_info_list.isNull()?0:snapshot_info_list->getSnapshotListSize();
}

int SnapshotTableModel::getColumnCount() const {
    return 3;
}

QString SnapshotTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Timestamp");
        break;
    case 2:
        result = QString("Work Concurrency");
        break;
    }
    return result;
}

QVariant SnapshotTableModel::getCellData(int row, int column) const {
    QVariant result;
    switch(column) {
    default:
        break;
    }
    return result;
}

QVariant SnapshotTableModel::getBackgroundForData(int row, int column) const {
    QVariant result;
    switch(column) {

    default:
        break;
    }
    return result;
}
