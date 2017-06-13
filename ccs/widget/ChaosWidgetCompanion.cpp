#include "ChaosWidgetCompanion.h"
#include "../error/ErrorManager.h"

#include <QDebug>

ChaosWidgetCompanion::ChaosWidgetCompanion():
    api_submitter(this){}

ChaosWidgetCompanion::~ChaosWidgetCompanion() {}

void ChaosWidgetCompanion::initChaosContent() {}

void ChaosWidgetCompanion::deinitChaosContent() {}

void ChaosWidgetCompanion::updateChaosContent() {}

void ChaosWidgetCompanion::setNodeUID(const QString& node_uid) {
    p_node_uid = node_uid;
}
const QString& ChaosWidgetCompanion::nodeUID() {
    return p_node_uid;
}
void ChaosWidgetCompanion::submitApiResult(const QString& api_tag,
                                           chaos::metadata_service_client::api_proxy::ApiProxyResult api_result) {
    //call api
    api_submitter.submitApiResult(api_tag,
                                  api_result);
}
void ChaosWidgetCompanion::onApiDone(const QString& tag,
                                     QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

}
void ChaosWidgetCompanion::onApiError(const QString& tag,
                                      QSharedPointer<chaos::CException> api_exception) {
    ErrorManager::getInstance()->submiteError(api_exception);
}
void ChaosWidgetCompanion::onApiTimeout(const QString& tag) {}
void ChaosWidgetCompanion::apiHasStarted(const QString& api_tag) {}
void ChaosWidgetCompanion::apiHasEnded(const QString& api_tag) {}
