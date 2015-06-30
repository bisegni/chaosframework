#include "ChaosLabel.h"

using namespace chaos::metadata_service_client::monitor_system;

ChaosLabel::ChaosLabel(QWidget * parent,
                       Qt::WindowFlags f):
    QLabel(parent, f),
    monitoring(false){

}

ChaosLabel::~ChaosLabel() {

}

void ChaosLabel::setNodeUniqueID(const QString& node_uid) {
    if(node_uid.compare(p_node_uid) == 0 ||
            monitoring) return;
    emit nodeUniqueIDChanged(p_node_uid,
                             node_uid);
    p_node_uid = node_uid;
}

QString ChaosLabel::nodeUniqueID() {
    return p_node_uid;
}

void ChaosLabel::setAttributeName(const QString& attribute_name) {
    if(attribute_name.compare(p_attribute_name) == 0 ||
            monitoring) return;
    emit attributeNameChanged(p_node_uid,
                              p_attribute_name,
                              attribute_name);
    p_attribute_name = attribute_name;
}

QString ChaosLabel::attributeName() {
    return p_attribute_name;
}

void ChaosLabel::setAttributeType(chaos::DataType::DataType attribute_type) {
    if(attribute_type == p_attribute_type ||
            monitoring) return;
    emit attributeTypeChanged(p_attribute_type,
                              attribute_type);
    p_attribute_type = attribute_type;
}

chaos::DataType::DataType ChaosLabel::attributeType() {
    return p_attribute_type;
}

void ChaosLabel::startMonitoring() {
    monitoring = true;
}

void ChaosLabel::stopMonitoring() {
    monitoring = false;
}

void ChaosLabel::valueUpdated(const QString& node_uid,
                              const QString& attribute_name,
                              uint64_t timestamp,
                              const QVariant& attribute_value) {

}

boost::shared_ptr<AbstractTSTaggedAttributeHandler>
ChaosLabel::getChaosAttributeHandlerForType(chaos::DataType::DataType chaos_type) {
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
        break;
    }
    return boost::shared_ptr<AbstractTSTaggedAttributeHandler>();
}

//slots hiding
void	ChaosLabel::clear(){QLabel::clear();}
void	ChaosLabel::setMovie(QMovie * movie){QLabel::setMovie(movie);}
void	ChaosLabel::setNum(int num){QLabel::setNum(num);}
void	ChaosLabel::setNum(double num){QLabel::setNum(num);}
void	ChaosLabel::setPicture(const QPicture & picture){QLabel::picture();}
void	ChaosLabel::setPixmap(const QPixmap &pixmap){QLabel::setPixmap(pixmap);}
void	ChaosLabel::setText(const QString &string){QLabel::setText(string);}
