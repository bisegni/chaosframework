#include "CNodeHealthLabel.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::node_monitor;

CNodeHealthLabel::CNodeHealthLabel(QWidget *parent) :
    QLabel(parent),
    ChaosMonitorWidgetCompanion(ControllerTypeNode, this),
    p_health_attribute(HealthTimestamp){}

CNodeHealthLabel::~CNodeHealthLabel() {
    deinitChaosContent();
}
void CNodeHealthLabel::initChaosContent() {
    trackNode();
}

void CNodeHealthLabel::deinitChaosContent() {
    untrackNode();
}

void CNodeHealthLabel::updateChaosContent() {

}

void CNodeHealthLabel::setHealthAttribute(CNodeHealthLabel::HealthAttribute health_attribute) {
    p_health_attribute = health_attribute;
}

CNodeHealthLabel::HealthAttribute CNodeHealthLabel::healthAttribute() {
    return p_health_attribute;
}

void CNodeHealthLabel::setCustomHealthAttribute(const QString& health_attribute) {
    p_custom_health_attribute = health_attribute;
}

QString CNodeHealthLabel::customHealthAttribute() {
    return p_custom_health_attribute;
}

void CNodeHealthLabel::nodeChangedOnlineState(const std::string& node_uid,
                                              OnlineState old_state,
                                              OnlineState new_state) {
    online_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
}

void CNodeHealthLabel::nodeChangedInternalState(const std::string& node_uid,
                                                const std::string& old_state,
                                                const std::string& new_state) {
    internal_state = QString::fromStdString(new_state);
    QMetaObject::invokeMethod(this,
                              "updateStateUI",
                              Qt::QueuedConnection);
}

void CNodeHealthLabel::nodeChangedErrorInformation(const std::string& node_uid,
                                                   const ErrorInformation& old_error_information,
                                                   const ErrorInformation& new_error_information) {
    error_information = new_error_information;
    QMetaObject::invokeMethod(this,
                              "updateErrorUI",
                              Qt::QueuedConnection);
}
#define CHECK_HATTRIBUTE_AND_GET(x,t,st)\
    if(map_health_dataset.count(x)) {\
    text_to_show = QString::st(map_health_dataset[x].t());\
    }

void CNodeHealthLabel::nodeChangedHealthDataset(const std::string& node_uid,
                                                MapDatasetKeyValues& map_health_dataset) {
    //set text for the label
    QString text_to_show = "-----";
    switch (healthAttribute()) {
    case HealthTimestamp:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, asInt64, number);
        break;
    case HealthUptime:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, asInt64, number);
        break;
    case HealthUserTime:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, asDouble, number);
        break;
    case HealthSystemTime:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME, asDouble, number);
        break;
    case HealthSwapTime:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME, asInt64, number);
        break;
    case HealthOnlineState:
        if(map_health_dataset.count(NodeMonitorHandler::MAP_KEY_ONLINE_STATE)) {
            switch((OnlineState)map_health_dataset[NodeMonitorHandler::MAP_KEY_ONLINE_STATE].asInt32()) {
            case OnlineStateNotFound:
                text_to_show = "State not found";
                break;
            case OnlineStateUnknown:
                text_to_show = "State Unknown";
                break;
            case OnlineStateOFF:
                text_to_show = "Offline";
                break;
            case OnlineStateON:
                text_to_show = "Online";
                break;
            }
        }
        break;
    case HealthOperationalState:
        CHECK_HATTRIBUTE_AND_GET(NodeHealtDefinitionKey::NODE_HEALT_STATUS, asString, fromStdString);
        break;
    case HealthCustomAttribute:
        if(map_health_dataset.count(customHealthAttribute().toStdString())) {
            CDataVariant custom_attribute_variant = map_health_dataset[customHealthAttribute().toStdString()];
            switch(custom_attribute_variant.getType()) {
            case DataType::TYPE_BOOLEAN:
                text_to_show = custom_attribute_variant.asBool()?"True":"False";
                break;
            case DataType::TYPE_INT32:
                text_to_show = QString::number(custom_attribute_variant.asInt32());
                break;
            case DataType::TYPE_INT64:
                text_to_show = QString::number(custom_attribute_variant.asInt64());
                break;
            case DataType::TYPE_DOUBLE:
                text_to_show = QString::number(custom_attribute_variant.asDouble());
                break;
            case DataType::TYPE_STRING:
                text_to_show = QString::fromStdString(custom_attribute_variant.asString());
                break;
            case DataType::TYPE_BYTEARRAY:
                CDataBuffer data_buffer = custom_attribute_variant.asCDataBuffer();
                QByteArray byte_array =  QByteArray::fromRawData(data_buffer.getBuffer(),data_buffer.getBufferSize());
                text_to_show = byte_array.toBase64();
                break;
            }
        } else {
            text_to_show = tr("HAttribute Not Found!");
        }
        break;
    default:
        break;
    }

    QMetaObject::invokeMethod(this,
                              "updateLabelUI",
                              Qt::QueuedConnection,
                              Q_ARG(QString, text_to_show));
}

void CNodeHealthLabel::updateStateUI() {
    bool in_error = internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0;

    if(in_error) {
        setStyleSheet("QLabel { color : #FF7C00; }");
    } else {
        switch (online_state) {
        case OnlineStateNotFound:
        case OnlineStateUnknown:
            setStyleSheet("QLabel { color : gray; }");
            break;
        case OnlineStateON:
            setStyleSheet("QLabel { color : #4EB66B; }");
            break;
        case OnlineStateOFF:
            setStyleSheet("QLabel { color : #E65566; }");
        default:
            break;
        }
    }
}

void CNodeHealthLabel::updateErrorUI() {
    bool in_error = internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            internal_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0;
    if(in_error){
        setToolTip(QString("Error Number: %1\nError Message:%2\nError Domain:%3").arg(QString::number(error_information.error_code),
                                                                                      QString::fromStdString(error_information.error_message),
                                                                                      QString::fromStdString(error_information.error_domain)));
    } else {
        setToolTip("");
    }
}

void CNodeHealthLabel::updateLabelUI(const QString& label_text) {
    setText(label_text);
}

//slots hiding
void	CNodeHealthLabel::clear(){QLabel::clear();}
void	CNodeHealthLabel::setMovie(QMovie * movie){QLabel::setMovie(movie);}
void	CNodeHealthLabel::setNum(int num){QLabel::setNum(num);}
void	CNodeHealthLabel::setNum(double num){QLabel::setNum(num);}
void	CNodeHealthLabel::setPicture(const QPicture & picture){QLabel::picture();}
void	CNodeHealthLabel::setPixmap(const QPixmap &pixmap){QLabel::setPixmap(pixmap);}
