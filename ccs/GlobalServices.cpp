#include "GlobalServices.h"

GlobalServices::GlobalServices() {

}

HealtMonitorWidget *GlobalServices::currentNodeMonitor() {
    return current_node_monitor;
}

void GlobalServices::setCurrentNodeMonitor(HealtMonitorWidget *monitor) {
    current_node_monitor = monitor;
}
