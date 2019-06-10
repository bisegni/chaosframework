#ifndef ChaosMonitorWidgetCompanion_H
#define ChaosMonitorWidgetCompanion_H

#include "ChaosWidgetCompanion.h"

class ChaosMonitorWidgetCompanion:
        public ChaosWidgetCompanion {
public:
    ChaosMonitorWidgetCompanion();

    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerType _monitor_controller_type,
                                chaos::metadata_service_client::node_monitor::NodeMonitorHandler *_monitor_handler);
    ~ChaosMonitorWidgetCompanion();

    QString datasetValueToLabel(const QString &attribute,
                                chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& map_health_dataset,
                                unsigned int double_precision = 2);

    void setStyleSheetColorForOnlineState(chaos::metadata_service_client::node_monitor::OnlineState online_state,
                                          QWidget *widget);
    protected:
        bool trackNode();

    bool untrackNode();
private:

    bool changeKeyConsumerQuantumSlot(int old_quantum_multiplier,
                                      int new_quantum_multiplier);

    chaos::metadata_service_client::node_monitor::NodeMonitorHandler *monitor_handler;

    const chaos::metadata_service_client::node_monitor::ControllerType monitor_controller_type;

    bool is_tracking;

};

#endif // ChaosMonitorWidgetCompanion_H
