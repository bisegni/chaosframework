#ifndef GLOBALSERVICES_H
#define GLOBALSERVICES_H

#include "monitor/healt/HealtMonitorWidget.h"

#include <chaos/common/utility/Singleton.h>

class GlobalServices:
        public chaos::common::utility::Singleton<GlobalServices> {
    friend class Singleton<GlobalServices>;
    friend class MainController;
public:
    HealtMonitorWidget *currentNodeMonitor();
private:
    GlobalServices();

    void setCurrentNodeMonitor(HealtMonitorWidget *monitor);

    HealtMonitorWidget *current_node_monitor;
};

#endif // GLOBALSERVICES_H
