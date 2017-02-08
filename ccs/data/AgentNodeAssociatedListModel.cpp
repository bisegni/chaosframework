#include "AgentNodeAssociatedListModel.h"
#include "../metatypes.h"
#include "delegate/TwoLineInformationItem.h"

AgentNodeAssociatedListModel::AgentNodeAssociatedListModel():
    api_submitter(this){}

void AgentNodeAssociatedListModel::setAgent(const QString& agent_uid) {

}

int AgentNodeAssociatedListModel::getRowCount() const {
    return result_found.size();
}
QVariant AgentNodeAssociatedListModel::getRowData(int row) const {
    //    QSharedPointer<TwoLineInformationItem> cmd_desc(new TwoLineInformationItem(node_uid,
    //                                                                               QString("Type:%1 Heartbeat:%2 status:%3").arg(node_type,
    //                                                                                                                             node_health_ts,
    //                                                                                                                             node_health_status),
    //                                                                               QVariant::fromValue(found_node)));
    //    return QVariant::fromValue(cmd_desc);
    return QVariant();
}

QVariant AgentNodeAssociatedListModel::getUserData(int row) const {

}

void AgentNodeAssociatedListModel::onApiDone(const QString& tag,
                                             QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

}
