#include "ChaosDatasetLabel.h"
using namespace chaos::metadata_service_client;

ChaosDatasetLabel::ChaosDatasetLabel(QWidget *parent) :
    ChaosLabel(parent) {
    setDataset(DatasetOutput);
}

ChaosDatasetLabel::~ChaosDatasetLabel() {
    stopMonitoring();
}

ChaosDatasetLabel::Dataset ChaosDatasetLabel::dataset() {
    return p_dataset;
}

void ChaosDatasetLabel::setDataset(ChaosDatasetLabel::Dataset dataset) {
    if(dataset == p_dataset ||
            monitoring) return;
    emit datasetChanged(p_dataset,
                        dataset);
    p_dataset = dataset;
}

unsigned int ChaosDatasetLabel::getChaosDataset() {
    switch(dataset()) {
    case DatasetOutput:
        return chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
    case DatasetInput:
        return chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
    case DatasetCustom:
        return chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM;
    case DatasetSystem:
        return chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM;
    }
}

void ChaosDatasetLabel::valueUpdated(const QString& node_uid,
                                     const QString& attribute_name,
                                     uint64_t timestamp,
                                     const QVariant& attribute_value) {
    ChaosLabel::valueUpdated(node_uid,
                             attribute_name,
                             timestamp,
                             attribute_value);
}

int ChaosDatasetLabel::startMonitoring() {
    int err = 0;
    bool ok = true;
    if(err = ChaosLabel::startMonitoring()){
        stopMonitoring();
        return err;
    }
    //we can stop
    handler_sptr = getChaosAttributeHandlerForType(attributeType(), ok);
    if(!ok) {
        stopMonitoring();
        return -5;
    }

    monitor_system::AbstractQuantumTSTaggedAttributeHandler *handler_ptr = dynamic_cast<monitor_system::AbstractQuantumTSTaggedAttributeHandler*>(handler_sptr.get());
    if(handler_ptr == NULL) {
        stopMonitoring();
        return -6;
    }
    //connect the handler
    connect(handler_sptr.get(),
            SIGNAL(valueUpdated(QString,QString,uint64_t,QVariant)),
            SLOT(valueUpdated(QString,QString,uint64_t,QVariant)));

    if(!ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForDataset(nodeUniqueID().toStdString(),
                                                                                    getChaosDataset(),
                                                                                    20,
                                                                                    handler_ptr)){
        stopMonitoring();
        return -7;
    }
    monitoring = true;
    return 0;
}

int ChaosDatasetLabel::stopMonitoring() {
    int err = 0;
    if(err = ChaosLabel::stopMonitoring()){
        return err;
    }
    //we can stop
    monitor_system::AbstractQuantumTSTaggedAttributeHandler *handler_ptr = dynamic_cast<monitor_system::AbstractQuantumTSTaggedAttributeHandler*>(handler_sptr.get());
    if(!ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForDataset(nodeUniqueID().toStdString(),
                                                                                       getChaosDataset(),
                                                                                       20,
                                                                                       handler_ptr)) {
        return -5;
    }
    monitoring = false;
    return 0;
}

void ChaosDatasetLabel::setLabelValueShowTrackStatus(bool label_value_show_track_status) {
    ChaosLabel::setLabelValueShowTrackStatus(false);
}

boost::shared_ptr<AbstractTSTaggedAttributeHandler>
ChaosDatasetLabel::getChaosAttributeHandlerForType(ChaosDataType chaos_type, bool& ok) {
    ok = !attributeName().isEmpty();
    if(!ok) return boost::shared_ptr<AbstractTSTaggedAttributeHandler>();
    switch(attributeType()) {
    case chaos::DataType::TYPE_BOOLEAN:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedBoolAttributeHandler(attributeName(),
                                                                                                           chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_INT32:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedInt32AttributeHandler(attributeName(),
                                                                                                            chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_INT64:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedInt64AttributeHandler(attributeName(),
                                                                                                            chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_DOUBLE:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedDoubleAttributeHandler(attributeName(),
                                                                                                             chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_STRING:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedStringAttributeHandler(attributeName(),
                                                                                                             chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_BYTEARRAY:
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedBinaryAttributeHandler(attributeName(),
                                                                                                             chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    default:
        ok = false;
        return boost::shared_ptr<AbstractTSTaggedAttributeHandler>();
        break;
    }
}
