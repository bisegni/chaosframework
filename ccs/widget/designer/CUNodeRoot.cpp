#include "CUNodeRoot.h"
#include "../../error/ErrorManager.h"

#include <QDebug>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

CUNodeRoot::CUNodeRoot(QString device_id, QObject *parent) :
    QObject(parent),
    api_submitter(this),
    m_device_id(device_id){}


void CUNodeRoot::setCurrentAttributeValue(int dataset_type,
                                          QString attribute_name,
                                          QVariant attribute_value) {
    emit updateDatasetAttribute(dataset_type,
                                attribute_name,
                                attribute_value);
}

void CUNodeRoot::setOnlineState(int state) {
    emit updateOnlineState(state);
}

void CUNodeRoot::init(){}

void CUNodeRoot::start(){}

void CUNodeRoot::stop() {}

void CUNodeRoot::deinit() {}

void CUNodeRoot::onApiDone(const QString& /*tag*/,
                           QSharedPointer<chaos::common::data::CDataWrapper> /*api_result*/) {

}
void CUNodeRoot::onApiError(const QString& /*tag*/,
                            QSharedPointer<chaos::CException> api_exception) {
    ErrorManager::getInstance()->submiteError(api_exception);
}
void CUNodeRoot::onApiTimeout(const QString& /*tag*/) {}
void CUNodeRoot::apiHasStarted(const QString& /*api_tag*/) {}
void CUNodeRoot::apiHasEnded(const QString& /*api_tag*/) {}
void CUNodeRoot::apiHasEndedWithError(const QString& /*tag*/,
                                      const QSharedPointer<chaos::CException> /*api_exception*/){}


void CUNodeRoot::attributeChangeSetUpdated(QString /*device_id*/,
                                           QString attribute_name,
                                           QVariant attribute_value) {
    qDebug() << "CUNodeRoot::attributeChangeSetUpdated:" << m_device_id<< ":" << attribute_name << "=" << attribute_value.toString();
    map_attribute_value.remove(attribute_name);
    map_attribute_value.insert(attribute_name, attribute_value);
}

void CUNodeRoot::attributeChangeSetClear(QString /*device_id*/,
                                         QString attribute_name) {
    qDebug() << "CUNodeRoot::attributeChangeSetClear:" << m_device_id<< ":" << attribute_name;
    map_attribute_value.remove(attribute_name);
}

void CUNodeRoot::commitChangeSet() {
    qDebug() << "CUNodeRoot::commitChangeSet:" << m_device_id;
    ChaosSharedPtr<ControlUnitInputDatasetChangeSet> attribute_change_set = ChaosMakeSharedPtr<ControlUnitInputDatasetChangeSet>(m_device_id.toStdString());
    QMapIterator<QString, QVariant> i(map_attribute_value);
    while (i.hasNext()) {
        i.next();
        attribute_change_set->change_set.push_back(ChaosMakeSharedPtr<InputDatasetAttributeChangeValue>(i.key().toStdString(),
                                                                                                       i.value().toString().toStdString()));
    }
    if(attribute_change_set->change_set.size()) {
        std::vector<ChaosSharedPtr<ControlUnitInputDatasetChangeSet>> change_vec;
        change_vec.push_back(attribute_change_set);
        //!send message to mds
        api_submitter.submitApiResult("APPLY-CHANGESET",
                                      GET_CHAOS_API_PTR(SetInputDatasetAttributeValues)->execute(change_vec));
    }

    //signal commit operation
    emit changeSetCommitted();
}
