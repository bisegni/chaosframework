#include "CUNodeRoot.h"
#include "../../error/ErrorManager.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

CUNodeRoot::CUNodeRoot(QString device_id, QObject *parent) :
    QObject(parent),
    api_submitter(this),
    m_device_id(device_id){}


void CUNodeRoot::setCurrentAttributeValue(QString attribute_name,
                                          QVariant attribute_value) {
    emit updateDatasetAttribute(attribute_name,
                                attribute_value);
}


void CUNodeRoot::init(){

}

void CUNodeRoot::start(){

}

void CUNodeRoot::stop() {

}

void CUNodeRoot::deinit() {

}

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
