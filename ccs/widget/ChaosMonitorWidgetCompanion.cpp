#include "ChaosMonitorWidgetCompanion.h"

#include <QDebug>
#include <QWidget>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

ChaosMonitorWidgetCompanion::ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerType _monitor_controller_type,
                                                         node_monitor::NodeMonitorHandler *_monitor_handler):
    ChaosWidgetCompanion(),
    monitor_handler(_monitor_handler),
    monitor_controller_type(_monitor_controller_type),
    is_tracking(false) {}

ChaosMonitorWidgetCompanion::~ChaosMonitorWidgetCompanion() {}

bool ChaosMonitorWidgetCompanion::trackNode() {
    if(is_tracking) return true;
    return (is_tracking = ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                              monitor_controller_type,
                                                                              monitor_handler));
}

bool ChaosMonitorWidgetCompanion::untrackNode() {
    if(!is_tracking) return true;
    return (is_tracking = !ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(nodeUID().toStdString(),
                                                                                 monitor_controller_type,
                                                                                 monitor_handler));
}

QString ChaosMonitorWidgetCompanion::datasetValueToLabel(const QString& attribute,
                                                        MapDatasetKeyValues& map_health_dataset,
                                                         unsigned int double_precision) {
    QString result = "-----";
    if(map_health_dataset.count(attribute.toStdString())) {
        CDataVariant custom_attribute_variant = map_health_dataset[attribute.toStdString()];
        switch(custom_attribute_variant.getType()) {
        case DataType::TYPE_BOOLEAN:
            result = custom_attribute_variant.asBool()?"True":"False";
            break;
        case DataType::TYPE_INT32:
            result = QString::number(custom_attribute_variant.asInt32());
            break;
        case DataType::TYPE_INT64:
            result = QString::number(custom_attribute_variant.asInt64());
            break;
        case DataType::TYPE_DOUBLE:
            result = QString::number(custom_attribute_variant.asDouble(), 'f', double_precision);
            break;
            case DataType::TYPE_JSONOBJ:
        case DataType::TYPE_STRING:
            result = QString::fromStdString(custom_attribute_variant.asString());
            break;
        case DataType::TYPE_BYTEARRAY:
            const CDataBuffer *data_buffer = custom_attribute_variant.asCDataBuffer();
            QByteArray byte_array = QByteArray::fromRawData(data_buffer->getBuffer(), data_buffer->getBufferSize());
            result = byte_array.toBase64();
            break;
        }
    } else {
        qDebug() << " Attribute " << attribute << " not found on dataset";
    }
    return result;
}

void ChaosMonitorWidgetCompanion::setStyleSheetColorForOnlineState(OnlineState online_state,
                                      QWidget *widget) {
    switch (online_state) {
    case OnlineStateNotFound:
    case OnlineStateUnknown:
        widget->setStyleSheet("QLabel { color : gray; }");
        break;
    case OnlineStateON:
        widget->setStyleSheet("QLabel { color : #4EB66B; }");
        break;
    case OnlineStateOFF:
        widget->setStyleSheet("QLabel { color : #E65566; }");
    default:
        break;
    }
}
