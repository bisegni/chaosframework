#include "CLedIndicatorHealt.h"
#include <chaos/common/chaos_constants.h>

#include <QDateTime>
#include <QDebug>

#define RETRY_TIME_FOR_OFFLINE 6

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

CLedIndicatorHealt::CLedIndicatorHealt(QWidget *parent):
    LedIndicatorWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNode, this),
    no_ts(new QIcon(":/images/white_circle_indicator.png")),
    timeouted(new QIcon(":/images/red_circle_indicator.png")),
    alive(new QIcon(":/images/green_circle_indicator.png")) {
    addState(0, no_ts);
    addState(1, timeouted);
    addState(2, alive);
}

CLedIndicatorHealt::~CLedIndicatorHealt() {
    deinitChaosContent();
}
void CLedIndicatorHealt::initChaosContent() {
    trackNode();
}

void CLedIndicatorHealt::deinitChaosContent() {
    untrackNode();
}

void CLedIndicatorHealt::updateChaosContent() {

}

void CLedIndicatorHealt::updateStatusInfo() {
    switch(alive_state) {
    case chaos::metadata_service_client::node_monitor::OnlineStatusNotFound:
    case chaos::metadata_service_client::node_monitor::OnlineStatusUnknown:
        setState(0);
        break;
    case chaos::metadata_service_client::node_monitor::OnlineStatusOFF:
        setState(1);
        break;
    case chaos::metadata_service_client::node_monitor::OnlineStatusON:
        setState(2);
        break;
    }
}

void CLedIndicatorHealt::nodeChangedOnlineStatus(const std::string& node_uid,
                                                 chaos::metadata_service_client::node_monitor::OnlineStatus old_status,
                                                 chaos::metadata_service_client::node_monitor::OnlineStatus new_status) {
    alive_state = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
    emit changedOnlineStatus(nodeUID(),
                             alive_state);
}

void CLedIndicatorHealt::nodeChangedProcessResource(const std::string& node_uid,
                                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res) {

}

void CLedIndicatorHealt::nodeChangedErrorInformation(const std::string& node_uid,
                                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& old_error_information,
                                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& new_error_information) {

}

void CLedIndicatorHealt::handlerHasBeenRegistered(const std::string& node_uid,
                                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_status) {
    alive_state = current_health_status.online_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}
