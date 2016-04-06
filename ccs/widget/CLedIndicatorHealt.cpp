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

void CLedIndicatorHealt::updateUIStatus() {
    switch(alive_state) {
    case chaos::metadata_service_client::node_monitor::OnlineStateNotFound:
    case chaos::metadata_service_client::node_monitor::OnlineStateUnknown:
        setState(0);
        break;
    case chaos::metadata_service_client::node_monitor::OnlineStateOFF:
        setState(1);
        break;
    case chaos::metadata_service_client::node_monitor::OnlineStateON:
        setState(2);
        break;
    }
}

void CLedIndicatorHealt::nodeChangedOnlineState(const std::string& node_uid,
                                                 chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                                 chaos::metadata_service_client::node_monitor::OnlineState new_state) {
    alive_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
    emit changedOnlineStatus(nodeUID(),
                             alive_state);
}

void CLedIndicatorHealt::handlerHasBeenRegistered(const std::string& node_uid,
                                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_status,
                                                  chaos::metadata_service_client::node_monitor::MapDatasetKeyValues &map_health_dataset) {
    alive_state = current_health_status.online_state;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}
