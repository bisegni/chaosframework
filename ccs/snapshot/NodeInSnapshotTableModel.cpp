#include "NodeInSnapshotTableModel.h"


#include <QColor>
#include <QDateTime>

using namespace chaos::metadata_service_client::api_proxy::service;

NodeInSnapshotTableModel::NodeInSnapshotTableModel(QObject *parent):
    ChaosAbstractListModel(parent) {

}

NodeInSnapshotTableModel::~NodeInSnapshotTableModel() {

}

void NodeInSnapshotTableModel::updateSnapshotList(std::unique_ptr<chaos::metadata_service_client::api_proxy::service::GetNodesForSnapshotHelper> _node_in_snapshot_helper) {
 beginResetModel();
 node_in_snapshot_helper = std::move(_node_in_snapshot_helper);
 endResetModel();
}

void NodeInSnapshotTableModel::reset() {
    beginResetModel();
    node_in_snapshot_helper.reset();
    endResetModel();
}

int NodeInSnapshotTableModel::getRowCount() const {
    return (node_in_snapshot_helper.get() == NULL)?0:node_in_snapshot_helper->getNodeListSize();
}

QVariant NodeInSnapshotTableModel::getRowData(int row) const {
    return QString::fromStdString(node_in_snapshot_helper->getNodeList()[row]);
}
QVariant NodeInSnapshotTableModel::getUserData(int row) const {
    return QString::fromStdString(node_in_snapshot_helper->getNodeList()[row]);
}

