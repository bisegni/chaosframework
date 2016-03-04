#ifndef CHAOSREADDATASETWIDGETCOMPANION_H
#define CHAOSREADDATASETWIDGETCOMPANION_H

#include "ChaosWidgetCompanion.h"

class ChaosReadDatasetWidgetCompanion:
        public ChaosWidgetCompanion,
        public chaos::metadata_service_client::monitor_system::QuantumSlotConsumer {
public:
    enum Dataset { DatasetOutput, DatasetInput, DatasetCustom, DatasetSystem, HelathDataset };

    ChaosReadDatasetWidgetCompanion();
    ~ChaosReadDatasetWidgetCompanion();

    Dataset dataset();
    void setDataset(Dataset dataset);

    quint32 schedulerSlot();
    void setSchedulerSlot(quint32 scheduler_slot);

protected:
    bool trackDataset(quint32 _scheduler_slot);

    bool untrackDataset(quint32 _scheduler_slot);

    void quantumSlotHasData(const std::string& key,
                            const chaos::metadata_service_client::monitor_system::KeyValue& value);
    void quantumSlotHasNoData(const std::string& key);

private:

    bool changeKeyConsumerQuantumSlot(int old_quantum_multiplier,
                                      int new_quantum_multiplier);

    quint32 p_scheduler_slot;
    Q_PROPERTY(quint32 scheduler_slot READ schedulerSlot WRITE setSchedulerSlot)

    Dataset p_dataset;
    Q_PROPERTY(Dataset dataset READ dataset WRITE setDataset)
    Q_ENUMS(Dataset)

    bool isTracking;

};

#endif // CHAOSREADDATASETWIDGETCOMPANION_H
