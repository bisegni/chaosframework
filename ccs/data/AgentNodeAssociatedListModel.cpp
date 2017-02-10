#include "AgentNodeAssociatedListModel.h"

#include <chaos_service_common/data/data.h>

#include <QDebug>
#include <QMimeData>
#include <QDataStream>

using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;

AgentNodeAssociatedListModel::AgentNodeAssociatedListModel():
    api_submitter(this){
}

void AgentNodeAssociatedListModel::setAgent(const QString& _agent_uid) {
    agent_uid = _agent_uid;
    updateList();
}

void AgentNodeAssociatedListModel::updateList() {
    api_submitter.submitApiResult("AgentNodeAssociatedListModel::setAgent",
                                  GET_CHAOS_API_PTR(agent::ListNodeForAgent)->execute(agent_uid.toStdString()));
}

int AgentNodeAssociatedListModel::getRowCount() const {
    return associated_nodes.size();
}
QVariant AgentNodeAssociatedListModel::getRowData(int row) const {
    return QVariant(QString::fromStdString(associated_nodes[row]));
}

QVariant AgentNodeAssociatedListModel::getUserData(int row) const {
    return QVariant(QString::fromStdString(associated_nodes[row]));
}

Qt::DropActions AgentNodeAssociatedListModel::supportedDropActions() const {
    return Qt::CopyAction;
}

QStringList AgentNodeAssociatedListModel::mimeTypes() const {
    QStringList types;
    types << "application/chaos-node-uid-type-list";
    return types;
}

Qt::ItemFlags AgentNodeAssociatedListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = ChaosAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

bool AgentNodeAssociatedListModel::canDropMimeData(const QMimeData *data,
                                                   Qt::DropAction action,
                                                   int row,
                                                   int column,
                                                   const QModelIndex &parent) {
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (!data->hasFormat("application/chaos-node-uid-type-list"))
        return false;

    if (column > 0)
        return false;

    return true;
}

bool AgentNodeAssociatedListModel::dropMimeData(const QMimeData *data,
                                                Qt::DropAction action,
                                                int row,
                                                int column,
                                                const QModelIndex &parent) {
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/chaos-node-uid-type-list"))
        return false;
    QByteArray encodedData = data->data("application/chaos-node-uid-type-list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QString node_uid;
        QString node_type;
        stream >> node_uid;
        stream >> node_type;

        //add association with default values for a unit server only nodes
        VectorAgentAssociation association_vector;
        if(node_type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
            qDebug() << "Associate node " << node_uid << " into agent " << agent_uid;
            //we cna create association
            AgentAssociation association;
            association.associated_node_uid = node_uid.toStdString();
            association_vector.push_back(association);
        }
        api_submitter.submitApiResult("AgentNodeAssociatedListModel::update",
                                      GET_CHAOS_API_PTR(agent::SaveNodeAssociation)->execute(agent_uid.toStdString(),
                                                                                             association_vector));
    }
    return true;
}

void AgentNodeAssociatedListModel::onApiDone(const QString& tag,
                                             QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("AgentNodeAssociatedListModel::setAgent") == 0) {
        beginResetModel();
        associated_nodes.clear();
        GET_CHAOS_API_PTR(agent::ListNodeForAgent)->deserialize(api_result.data(),
                                                                associated_nodes);
        endResetModel();
    } else {
        updateList();
    }
}
