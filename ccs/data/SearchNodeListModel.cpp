#include "SearchNodeListModel.h"
#include "../metatypes.h"
#include "delegate/TwoLineInformationItem.h"

#include <QDateTime>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

SearchNodeListModel::SearchNodeListModel():
    api_submitter(this){}

int SearchNodeListModel::getRowCount() const {
    return result_found.size();
}
QVariant SearchNodeListModel::getRowData(int row) const {
    if(result_found.size()==0) return QVariant();
    QSharedPointer<CDataWrapper> found_node = result_found[row];

    QString node_uid = QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    QString node_type =  QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
    QString node_health_ts("---");
    QString node_health_status("---");
    if(found_node->hasKey("health_stat") &&
            found_node->isCDataWrapperValue("health_stat")) {
        std::auto_ptr<CDataWrapper> health_stat(found_node->getCSDataValue("health_stat"));
        node_health_ts = QDateTime::fromMSecsSinceEpoch(health_stat->getUInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP), Qt::LocalTime).toString();
        node_health_status = QString::fromStdString(health_stat->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS));
    }

    QSharedPointer<TwoLineInformationItem> cmd_desc(new TwoLineInformationItem(node_uid,
                                                                               QString("Type:%1 Heartbeat:%2 status:%3").arg(node_type,
                                                                                                                             node_health_ts,
                                                                                                                             node_health_status),
                                                                               QVariant::fromValue(found_node)));
    return QVariant::fromValue(cmd_desc);
}

QVariant SearchNodeListModel::getUserData(int row) const {
    return QVariant();
}

bool SearchNodeListModel::isRowCheckable(int row) const {
    return false;
}

Qt::CheckState SearchNodeListModel::getCheckableState(int row)const {
    return Qt::Unchecked;
}

bool SearchNodeListModel::setRowCheckState(const int row, const QVariant& value) {
    return false;
}

void SearchNodeListModel::onApiDone(const QString& tag,
                                    QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("search_node") != 0) return;
    beginResetModel();
    //reset current page
    result_found.clear();

    if(!api_result.isNull() &&
            api_result->hasKey("node_search_result_page") &&
            api_result->isVectorValue("node_search_result_page")) {
        //we have result
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue("node_search_result_page"));

        if(arr->size()) {
            //get first element seq
            for(int i = 0;
                i < arr->size();
                i++) {
                result_found.append(QSharedPointer<CDataWrapper>(arr->getCDataWrapperElementAtIndex(i)));
            }
        }else{
            qDebug() << "No data found";
        }
    }
    endResetModel();
}

void SearchNodeListModel::searchNode(const QString& search_string,
                                     unsigned int search_type,
                                     bool is_alive) {
    api_submitter.submitApiResult("search_node",
                                  GET_CHAOS_API_PTR(node::NodeSearch)->execute(search_string.toStdString(),
                                                                               search_type,
                                                                               is_alive,
                                                                               0,
                                                                               1000));
}
