#include "ChaosReadDatasetWidgetCompanion.h"

#include <QDebug>
using namespace chaos::metadata_service_client;

ChaosReadDatasetWidgetCompanion::ChaosReadDatasetWidgetCompanion():
    ChaosWidgetCompanion(),
    p_scheduler_slot(10),
    isTracking(false) {}

ChaosReadDatasetWidgetCompanion::~ChaosReadDatasetWidgetCompanion() {}

ChaosReadDatasetWidgetCompanion::Dataset ChaosReadDatasetWidgetCompanion::dataset() {
    return p_dataset;
}

void ChaosReadDatasetWidgetCompanion::setDataset(ChaosReadDatasetWidgetCompanion::Dataset dataset) {
    if(isTracking) return;
    p_dataset = dataset;
}

quint32 ChaosReadDatasetWidgetCompanion::schedulerSlot() {
    return p_scheduler_slot;
}

void ChaosReadDatasetWidgetCompanion::setSchedulerSlot(quint32 scheduler_slot) {
    if(isTracking) {
        if(changeKeyConsumerQuantumSlot(p_scheduler_slot, scheduler_slot)) {
            p_scheduler_slot = scheduler_slot;
        }
    } else {
        p_scheduler_slot = scheduler_slot;
    }
}

bool ChaosReadDatasetWidgetCompanion::trackDataset(quint32 _scheduler_slot) {
    QString node_key = nodeUID();
    switch(dataset()) {
    case DatasetOutput:
        node_key.append(chaos::DataPackPrefixID::OUTPUT_DATASE_PREFIX);
        break;
    case DatasetInput:
        node_key.append(chaos::DataPackPrefixID::INPUT_DATASE_PREFIX);
        break;
    case DatasetCustom:
        node_key.append(chaos::DataPackPrefixID::CUSTOM_DATASE_PREFIX);
        break;
    case DatasetSystem:
        node_key.append(chaos::DataPackPrefixID::SYSTEM_DATASE_PREFIX);
        break;
    case HelathDataset:
        node_key.append(chaos::DataPackPrefixID::HEALTH_DATASE_PREFIX);
        break;

    }
    return ChaosMetadataServiceClient::getInstance()->addKeyConsumer(node_key.toStdString(),
                                                                     _scheduler_slot,
                                                                     this);
}

bool ChaosReadDatasetWidgetCompanion::untrackDataset(quint32 _scheduler_slot) {
    QString node_key = nodeUID();
    switch(dataset()) {
    case DatasetOutput:
        node_key.append(chaos::DataPackPrefixID::OUTPUT_DATASE_PREFIX);
        break;
    case DatasetInput:
        node_key.append(chaos::DataPackPrefixID::INPUT_DATASE_PREFIX);
        break;
    case DatasetCustom:
        node_key.append(chaos::DataPackPrefixID::CUSTOM_DATASE_PREFIX);
        break;
    case DatasetSystem:
        node_key.append(chaos::DataPackPrefixID::SYSTEM_DATASE_PREFIX);
        break;
    case HelathDataset:
        node_key.append(chaos::DataPackPrefixID::HEALTH_DATASE_PREFIX);
        break;

    }
    if(ChaosMetadataServiceClient::getInstance()->removeKeyConsumer(node_key.toStdString(),
                                                                    _scheduler_slot,
                                                                    this,
                                                                    false) == false){
        QuantumSlotConsumer::waitForCompletion();
    }
    return true;
}

bool ChaosReadDatasetWidgetCompanion::changeKeyConsumerQuantumSlot(int old_quantum_multiplier,
                                                                   int new_quantum_multiplier) {
    if(untrackDataset(old_quantum_multiplier)){
        trackDataset(new_quantum_multiplier);
    }
}

void ChaosReadDatasetWidgetCompanion::quantumSlotHasData(const std::string& key,
                                                         const monitor_system::KeyValue& value) {}

void ChaosReadDatasetWidgetCompanion::quantumSlotHasNoData(const std::string& key) {}
