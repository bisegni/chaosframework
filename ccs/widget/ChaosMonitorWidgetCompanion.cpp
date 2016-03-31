#include "ChaosMonitorWidgetCompanion.h"

#include <QDebug>
using namespace chaos::metadata_service_client;

ChaosMonitorWidgetCompanion::ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerType _monitor_controller_type,
                                                         node_monitor::NodeMonitorHandler *_monitor_handler):
    ChaosWidgetCompanion(),
    monitor_handler(_monitor_handler),
    monitor_controller_type(_monitor_controller_type),
    isTracking(false) {}

ChaosMonitorWidgetCompanion::~ChaosMonitorWidgetCompanion() {}

bool ChaosMonitorWidgetCompanion::trackDataset() {
    return ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                              monitor_controller_type,
                                                                              monitor_handler);
}

bool ChaosMonitorWidgetCompanion::untrackDataset() {
    return ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                                 monitor_controller_type,
                                                                                 monitor_handler);
}
