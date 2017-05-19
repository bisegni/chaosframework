#include "SearchNodeListModel.h"
#include "../metatypes.h"
#include "delegate/TwoLineInformationItem.h"

#include <QDateTime>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
#include <QMimeData>
#include <QDataStream>
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
        ChaosUniquePtr<chaos::common::data::CDataWrapper> health_stat(found_node->getCSDataValue("health_stat"));
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

Qt::ItemFlags SearchNodeListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

Qt::DropActions SearchNodeListModel::supportedDragActions() const {
    return Qt::CopyAction;
}

QMimeData *SearchNodeListModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *result = new QMimeData();
    QModelIndexList::const_iterator iter = indexes.begin();
    QModelIndexList::const_iterator iter_end = indexes.end();
    QByteArray encoded_data;
    QByteArray encoded_data_node_and_type;
    QDataStream stream(&encoded_data, QIODevice::WriteOnly);
    while(iter != iter_end){
        if(iter->isValid() == false) continue;
        QSharedPointer<TwoLineInformationItem> element = iter->data().value<QSharedPointer<TwoLineInformationItem> >();
        stream << element->title;
        iter++;
    }
    result->setData("application/chaos-node-uid-list", encoded_data);

    iter = indexes.begin();
    iter_end = indexes.end();
    QDataStream stream_node_type(&encoded_data_node_and_type, QIODevice::WriteOnly);
    while(iter != iter_end){
        if(iter->isValid() == false) continue;
        QSharedPointer<CDataWrapper> found_node = result_found[iter->row()];

        QString node_uid = QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        QString node_type =  QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
        stream_node_type << node_uid;
        stream_node_type << node_type;
        iter++;
    }
    result->setData("application/chaos-node-uid-type-list", encoded_data_node_and_type);
    return result;
}

QStringList SearchNodeListModel::mimeTypes() const {
    QStringList types;
    types << "application/chaos-node-uid-list";
    return types;
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
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue("node_search_result_page"));

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
