#include "ChaosMonitorWidgetCompanion.h"

#include <QDebug>
using namespace chaos::metadata_service_client;

ChaosMonitorWidgetCompanion::ChaosMonitorWidgetCompanion(node_monitor::NodeMonitorHandler *_monitor_handler):
    ChaosWidgetCompanion(),
    monitor_handler(_monitor_handler),
    isTracking(false) {}

ChaosMonitorWidgetCompanion::~ChaosMonitorWidgetCompanion() {}

ChaosMonitorWidgetCompanion::Dataset ChaosMonitorWidgetCompanion::dataset() {
    return p_dataset;
}

void ChaosMonitorWidgetCompanion::setDataset(ChaosMonitorWidgetCompanion::Dataset dataset) {
    if(isTracking) return;
    p_dataset = dataset;
}

bool ChaosMonitorWidgetCompanion::trackDataset() {
    return ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                              monitor_handler);
}

bool ChaosMonitorWidgetCompanion::untrackDataset() {
    return ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                                 monitor_handler);
}
