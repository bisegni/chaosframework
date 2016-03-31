#ifndef ChaosMonitorWidgetCompanion_H
#define ChaosMonitorWidgetCompanion_H

#include "ChaosWidgetCompanion.h"

class ChaosMonitorWidgetCompanion:
        public ChaosWidgetCompanion {
public:

    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerType _monitor_controller_type,
                                chaos::metadata_service_client::node_monitor::NodeMonitorHandler *_monitor_handler);
    ~ChaosMonitorWidgetCompanion();

protected:
    bool trackDataset();

    bool untrackDataset();
private:

    bool changeKeyConsumerQuantumSlot(int old_quantum_multiplier,
                                      int new_quantum_multiplier);

    chaos::metadata_service_client::node_monitor::NodeMonitorHandler *monitor_handler;

    const chaos::metadata_service_client::node_monitor::ControllerType monitor_controller_type;

    bool isTracking;

};

#endif // ChaosMonitorWidgetCompanion_H
