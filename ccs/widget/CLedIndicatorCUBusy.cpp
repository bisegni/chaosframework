#include "CLedIndicatorCUBusy.h"

#include <QDateTime>
#include <QDebug>

#define RETRY_TIME_FOR_OFFLINE 6

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::node_monitor;

CLedIndicatorCUBusy::CLedIndicatorCUBusy(QWidget *parent):
    StateImageIndicatorWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    busy(false),
    no_busy_icon(new QIcon(":/images/pause_black.png")),
    busy_icon(new QIcon(":/images/play_blue.png")) {
    addState(0, no_busy_icon);
    addState(1, busy_icon);
}

CLedIndicatorCUBusy::~CLedIndicatorCUBusy() {
    deinitChaosContent();
}
void CLedIndicatorCUBusy::initChaosContent() {
    trackNode();
}

void CLedIndicatorCUBusy::deinitChaosContent() {
    untrackNode();
}

void CLedIndicatorCUBusy::updateChaosContent() {

}

void CLedIndicatorCUBusy::updateUIStatus() {
    switch(alive_state) {
    case chaos::metadata_service_client::node_monitor::OnlineStateNotFound:
    case chaos::metadata_service_client::node_monitor::OnlineStateUnknown:
    case chaos::metadata_service_client::node_monitor::OnlineStateOFF:
        this->setEnabled(false);
        break;
    case chaos::metadata_service_client::node_monitor::OnlineStateON:
        this->setEnabled(true);
        break;
    }

    if(busy) {
        setState(1);
    } else {
        setState(0);
    }
}

void CLedIndicatorCUBusy::nodeChangedOnlineState(const std::string& node_uid,
                                                 chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                                 chaos::metadata_service_client::node_monitor::OnlineState new_state) {
    alive_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CLedIndicatorCUBusy::updatedDS(const std::string& control_unit_uid,
                                    int dataset_type,
                                    chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type != DatasetTypeSystem) return;
    if(dataset_key_values.count("busy") == 0) return;

    busy = dataset_key_values["busy"].asBool();
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CLedIndicatorCUBusy::noDSDataFound(const std::string& control_unit_uid,
                                        int dataset_type) {
    busy = false;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}
