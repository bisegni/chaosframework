#include "ChaosHealtLabel.h"

using namespace chaos::metadata_service_client;

ChaosHealtLabel::ChaosHealtLabel(QWidget *parent):
    ChaosLabel(parent) {

}

ChaosHealtLabel::~ChaosHealtLabel() {

}

int ChaosHealtLabel::startMonitoring() {
    int err = 0;
    bool ok = true;
    if((err = ChaosLabel::startMonitoring())){
        stopMonitoring();
        return err;
    }
    //we can stop
    handler_sptr = getAttributeHandlerForType(attributeType(), ok);
    if(!ok) {
        stopMonitoring();
        return -1;
    }

    connect(handler_sptr.get(),
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(valueUpdated(QString,QString,QVariant)));

    if(!ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                  20,
                                                                                  handler_sptr->getQuantumAttributeHandler())){
        stopMonitoring();
        return -2;
    }
    monitoring = true;
    return 0;
}

int ChaosHealtLabel::stopMonitoring() {
    int err = 0;
    if((err = ChaosLabel::stopMonitoring())){
        return err;
    }
    //we can stop
    if(!ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                     20,
                                                                                     handler_sptr->getQuantumAttributeHandler())) {
        return -1;
    }
    monitoring = false;
    return 0;
}

boost::shared_ptr<AbstractAttributeHandler>
ChaosHealtLabel::getAttributeHandlerForType(ChaosDataType chaos_type, bool& ok) {
    ok = !attributeName().isEmpty();
    if(!ok) return boost::shared_ptr<AbstractAttributeHandler>();
    switch(attributeType()) {
    case chaos::DataType::TYPE_BOOLEAN:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorBoolAttributeHandler(attributeName()));
    case chaos::DataType::TYPE_INT32:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorInt32AttributeHandler(attributeName()));
    case chaos::DataType::TYPE_INT64:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorInt64AttributeHandler(attributeName()));
    case chaos::DataType::TYPE_DOUBLE:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorDoubleAttributeHandler(attributeName()));
    case chaos::DataType::TYPE_STRING:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorStringAttributeHandler(attributeName()));
    case chaos::DataType::TYPE_BYTEARRAY:
        return boost::shared_ptr<AbstractAttributeHandler>(new MonitorBinaryAttributeHandler(attributeName()));
    default:
        ok = false;
        return boost::shared_ptr<AbstractAttributeHandler>();
        break;
    }
}
