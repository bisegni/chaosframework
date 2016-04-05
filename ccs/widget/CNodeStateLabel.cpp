#include "CNodeStateLabel.h"
using namespace chaos::metadata_service_client::node_monitor;

CNodeStateLabel::CNodeStateLabel(QWidget *parent) :
    QLabel(parent),
    ChaosMonitorWidgetCompanion(ControllerTypeNode, this) {}

CNodeStateLabel::~CNodeStateLabel() {
    deinitChaosContent();
}
void CNodeStateLabel::initChaosContent() {
    trackNode();
}

void CNodeStateLabel::deinitChaosContent() {
    untrackNode();
}

void CNodeStateLabel::updateChaosContent() {

}

void CNodeStateLabel::nodeChangedOnlineState(const std::string& node_uid,
                                             OnlineState old_state,
                                             OnlineState new_state) {
    online_state = new_status;
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
}

void CNodeStateLabel::nodeChangedInternalState(const std::string& node_uid,
                                               const std::string& old_state,
                                               const std::string& new_state) {
    internal_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
}

void CNodeStateLabel::nodeChangedErrorInformation(const std::string& node_uid,
                                                  const ErrorInformation& old_error_information,
                                                  const ErrorInformation& new_error_information) {
    error_information = new_error_information;
    QMetaObject::invokeMethod(this,
                              "updateErrorUI",
                              Qt::QueuedConnection);
}

void CNodeStateLabel::handlerHasBeenRegistered(const std::string& node_uid,
                                               const HealthInformation& current_health_state) {

}

void CNodeStateLabel::updateStateUI() {
    bool in_error = internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0;

    if(in_error) {
        setStyleSheet("QLabel { color : #FF7C00; }");
    } else {
        switch (online_state) {
        case OnlineStateNotFound:
        case OnlineStateUnknown:
            setStyleSheet("QLabel { color : gray; }");
            break;
        case OnlineStateON:
            setStyleSheet("QLabel { color : #4EB66B; }");
            break;
        case OnlineStateOFF:
            setStyleSheet("QLabel { color : #E65566; }");
        default:
            break;
        }
    }
}

void CNodeStateLabel::updateErrorUI() {
    bool in_error = internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0;
    if(in_error)
        if(value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE) &&
                value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE) &&
                value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE)) {
            //we need to show error
            setToolTip(QString("Error Number: %1\nError Message:%2\nError Domain:%3").arg(error_information.error_code,
                                                                                          error_information.error_message,
                                                                                          error_information.error_domain));
        }else {
            setToolTip("");
        }
}
