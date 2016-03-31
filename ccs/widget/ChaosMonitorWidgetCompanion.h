#ifndef ChaosMonitorWidgetCompanion_H
#define ChaosMonitorWidgetCompanion_H

#include "ChaosWidgetCompanion.h"

class ChaosMonitorWidgetCompanion:
        public ChaosWidgetCompanion {
public:
    enum Dataset { DatasetOutput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                   DatasetInput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                   DatasetCustom = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM,
                   DatasetSystem = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM};

    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::NodeMonitorHandler *_monitor_handler);
    ~ChaosMonitorWidgetCompanion();

    Dataset dataset();
    void setDataset(Dataset dataset);

protected:
    bool trackDataset();

    bool untrackDataset();
private:

    bool changeKeyConsumerQuantumSlot(int old_quantum_multiplier,
                                      int new_quantum_multiplier);

    chaos::metadata_service_client::node_monitor::NodeMonitorHandler *monitor_handler;

    Dataset p_dataset;
    Q_PROPERTY(Dataset dataset READ dataset WRITE setDataset)
    Q_ENUMS(Dataset)

    bool isTracking;

};

#endif // ChaosMonitorWidgetCompanion_H
