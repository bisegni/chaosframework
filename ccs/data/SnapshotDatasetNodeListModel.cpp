#include "SnapshotDatasetNodeListModel.h"

#include "../metatypes.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy::service;

SnapshotDatasetNodeListModel::SnapshotDatasetNodeListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    api_submitter(this){}

SnapshotDatasetNodeListModel::~SnapshotDatasetNodeListModel(){}

void SnapshotDatasetNodeListModel::updateDatasetListFor(const QString& node_name,
                                                        const QString& snapshot_name) {
    beginResetModel();
    dataset_in_snapshot.clear();
    api_submitter.submitApiResult("get_snapshot_value",
                                  GET_CHAOS_API_PTR(GetSnapshotDatasetForNode)->execute(snapshot_name.toStdString(),
                                                                                        node_name.toStdString()));
    endResetModel();
}

void SnapshotDatasetNodeListModel::clear() {
    beginResetModel();
    dataset_in_snapshot.clear();
    endResetModel();
}

int SnapshotDatasetNodeListModel::getRowCount() const {
    return dataset_in_snapshot.size();
}
QVariant SnapshotDatasetNodeListModel::getRowData(int row) const {
    return QString::fromStdString(dataset_in_snapshot[row].first);
}
QVariant SnapshotDatasetNodeListModel::getUserData(int row) const {
    return QVariant::fromValue<CDWShrdPtr>(dataset_in_snapshot[row].second);
}
bool SnapshotDatasetNodeListModel::isRowCheckable(int row) const {
    return false;
}
Qt::CheckState SnapshotDatasetNodeListModel::getCheckableState(int row)const {
    Qt::Unchecked;
}
bool SnapshotDatasetNodeListModel::setRowCheckState(const int row, const QVariant& value) {
    false;
}

void SnapshotDatasetNodeListModel::onApiDone(const QString& tag,
                                             QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    beginResetModel();
    GetSnapshotDatasetForNode::getAsMap(*api_result.data(),
                                        dataset_in_snapshot);
    endResetModel();
}
