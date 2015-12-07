#include "NodeListModel.h"

NodeListModel::NodeListModel(QObject *parent):
    ChaosAbstractListModel(parent) {

}

int NodeListModel::getRowCount() const {
    return list_node_uid.size();
}

QVariant NodeListModel::getRowData(int row) const {
    return list_node_uid[row];
}
QVariant NodeListModel::getUserData(int row) const {
    return list_node_uid[row];
}

void NodeListModel::addNode(const QString& new_node) {
    if(list_node_uid.contains(new_node)) return;
    beginResetModel();
    list_node_uid.push_back(new_node);
    endResetModel();
}

void NodeListModel::removeNode(const QString& new_node) {
    if(!list_node_uid.contains(new_node)) return;
    beginResetModel();
    list_node_uid.removeOne(new_node);
    endResetModel();
}

const QVector<QString>& NodeListModel::getAllNode() {
    return list_node_uid;
}
