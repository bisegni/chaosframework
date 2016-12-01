#include "SnapshotDatasetNodeListModel.h"

SnapshotDatasetNodeListModel::SnapshotDatasetNodeListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    api_submitter(this){

}


void SnapshotDatasetNodeListModel::updateDatasetListFor(const QString& node_name,
                          const QString& snapshot_name) {

}

int SnapshotDatasetNodeListModel::getRowCount() const {

}
QVariant SnapshotDatasetNodeListModel::getRowData(int row) const {

}
QVariant SnapshotDatasetNodeListModel::getUserData(int row) const {

}
bool SnapshotDatasetNodeListModel::isRowCheckable(int row) const {

}
Qt::CheckState SnapshotDatasetNodeListModel::getCheckableState(int row)const {

}
bool SnapshotDatasetNodeListModel::setRowCheckState(const int row, const QVariant& value) {

}

QSharedPointer<chaos::common::data::CDataWrapper> SnapshotDatasetNodeListModel::getDatasetByType(unsigned int dataset_type) {
    return dataset_in_snapshot[dataset_type];
}

void SnapshotDatasetNodeListModel::onApiDone(const QString& tag,
               QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
beginResetModel();
//DataPackCommonKey
endResetModel();
}
