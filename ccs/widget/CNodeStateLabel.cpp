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
    online_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
}

void CNodeStateLabel::nodeChangedInternalState(const std::string& node_uid,
                                               const std::string& old_state,
                                               const std::string& new_state) {
    internal_state = QString::fromStdString(new_state);
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
    online_state = current_health_state.online_state;
    internal_state = QString::fromStdString(current_health_state.internal_state);
    error_information = current_health_state.error_information;
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
    QMetaObject::invokeMethod(this,
                              "updateErrorUI",
                              Qt::QueuedConnection);
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

    setText(internal_state);
}

void CNodeStateLabel::updateErrorUI() {
    bool in_error = internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0;
    if(in_error){
        setToolTip(QString("Error Number: %1\nError Message:%2\nError Domain:%3").arg(QString::number(error_information.error_code),
                                                                                      QString::fromStdString(error_information.error_message),
                                                                                      QString::fromStdString(error_information.error_domain)));
    } else {
        setToolTip("");
    }
}

//slots hiding
void	CNodeStateLabel::clear(){QLabel::clear();}
void	CNodeStateLabel::setMovie(QMovie * movie){QLabel::setMovie(movie);}
void	CNodeStateLabel::setNum(int num){QLabel::setNum(num);}
void	CNodeStateLabel::setNum(double num){QLabel::setNum(num);}
void	CNodeStateLabel::setPicture(const QPicture & picture){QLabel::picture();}
void	CNodeStateLabel::setPixmap(const QPixmap &pixmap){QLabel::setPixmap(pixmap);}
