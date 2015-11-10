#include "ChaosLabel.h"

#include <QDateTime>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::monitor_system;

ChaosLabel::ChaosLabel(QWidget * parent,
                       Qt::WindowFlags f):
    QLabel(parent, f),
    monitoring(false),
    last_recevied_ts(0),
    zero_diff_count(0) {
    setTimeoutForAlive(6000);
    
    connect(&healt_status_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(valueUpdated(QString,QString,QVariant)));
    connect(&healt_status_handler,
            SIGNAL(valueNotFound(QString,QString)),
            SLOT(valueNotFound(QString,QString)));
    connect(&healt_heartbeat_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(valueUpdated(QString,QString,QVariant)));
}

ChaosLabel::~ChaosLabel() {
    stopMonitoring();
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

void ChaosLabel::setAttributeType(ChaosDataType attribute_type) {
    if(attribute_type == p_attribute_type ||
            monitoring) return;
    emit attributeTypeChanged(p_attribute_type,
                              attribute_type);
    p_attribute_type = attribute_type;
}

ChaosDataType ChaosLabel::attributeType() {
    return p_attribute_type;
}

void ChaosLabel::setTimeoutForAlive(unsigned int timeout_for_alive) {
    p_timeout_for_alive = timeout_for_alive;
}

unsigned int ChaosLabel::timeoutForAlive() {
    return p_timeout_for_alive;
}

void ChaosLabel::setTrackStatus(bool track_status) {
    //do nothing if monitor is activated or the state is the same
    if(monitoring || p_track_status == track_status) return;
    p_track_status = track_status;
}

bool ChaosLabel::trackStatus() {
    return p_track_status;
}

void ChaosLabel::setLabelValueShowTrackStatus(bool label_value_show_track_status) {
    p_label_value_show_track_status = label_value_show_track_status;
}

bool ChaosLabel::labelValueShowTrackStatus() {
    return p_label_value_show_track_status;
}

bool labelValueShowTrackStatus();

int ChaosLabel::startMonitoring() {
    if(monitoring) return -1;
    monitoring = true;
    if(trackStatus()) {
        if(!ChaosMetadataServiceClient::getInstance()->addKeyConsumer(ChaosMetadataServiceClient::getInstance()->getHealtKeyFromGeneralKey(nodeUniqueID().toStdString()),
                                                                      20,
                                                                      this)) {
            return -2;
        }
    }
    return 0;
}

int ChaosLabel::stopMonitoring() {
    if(!monitoring) return -1;
    monitoring = false;
    if(trackStatus()) {
        if(!ChaosMetadataServiceClient::getInstance()->removeKeyConsumer(ChaosMetadataServiceClient::getInstance()->getHealtKeyFromGeneralKey(nodeUniqueID().toStdString()),
                                                                         20,
                                                                         this)) {
            return -2;
        }
    }
    return 0;
}

void ChaosLabel::_updateStatusColor() {
    if(!monitoring || !trackStatus()) return;
    bool offline = (zero_diff_count > 3) || (last_recevied_ts = 0);
    if(!offline) {
        if(last_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
                last_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0) {
            setStyleSheet("QLabel { color : #FF7C00; }");
        }else{
            setStyleSheet("QLabel { color : #4EB66B; }");
        }
    } else {
        if(last_recevied_ts == 0) {
            setStyleSheet("QLabel { color : gray; }");
        } else {
            setStyleSheet("QLabel { color : #E65566; }");
        }
    }
}

void ChaosLabel::valueUpdated(const QString& _node_uid,
                              const QString& _attribute_name,
                              const QVariant& _attribute_value) {
    if(_attribute_name.compare(attributeName()) == 0) {
        //we have a value given by an handler that doesn't expose the timestamp
        setText(_attribute_value.toString());
    }
}

void ChaosLabel::valueNotFound(const QString& _node_uid,
                               const QString& _attribute_name) {
}

void ChaosLabel::valueUpdated(const QString& _node_uid,
                              const QString& _attribute_name,
                              uint64_t _timestamp,
                              const QVariant& _attribute_value) {
    //write the value
    if(_attribute_name.compare(attributeName()) == 0) {
        //we have a value given by an handler that doesn't expose the timestamp
        setText(_attribute_value.toString());
    }
}

void ChaosLabel::quantumSlotHasData(const std::string& key, const KeyValue& value) {
    if(!value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) ||
            !value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS)) return;

    uint64_t received_ts = value->getUInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP);
    uint64_t time_diff = last_recevied_ts - received_ts;
    last_status = QString::fromStdString(value->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS));
    
    if(time_diff > 0) {
        //setStyleSheet("QLabel { color : #4EB66B; }");
        zero_diff_count = 0;
    } else {
        if(++zero_diff_count > 3) {
            //timeouted
            //setStyleSheet("QLabel { color : #E65566; }");
        } else {
            //in this case we do nothing perhaps we can to fast to check
        }
    }

    //write the value
    if(last_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
            last_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0) {
        //we need to show error
        const QString err_num = QString::number(value->getInt32Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE));
        const QString err_str = QString::fromStdString(value->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE));
        const QString err_dom = QString::fromStdString(value->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN));
        const QString error_tooltip = QString("Error Number: %1\nError Message:%2\nError Domain:%3").arg(err_num,err_str,err_dom);
        setToolTip(error_tooltip);
    } else {
        //show status also on label
        setToolTip(last_status);
    }
    
    if(labelValueShowTrackStatus()) {
        setText(last_status);
    }

    //update color
    _updateStatusColor();
    last_recevied_ts = received_ts;
}

void ChaosLabel::quantumSlotHasNoData(const std::string& key) {
    last_recevied_ts = zero_diff_count = 0;
    //setStyleSheet("QLabel { color : gray; }");
    _updateStatusColor();
}

//slots hiding
void	ChaosLabel::clear(){QLabel::clear();}
void	ChaosLabel::setMovie(QMovie * movie){QLabel::setMovie(movie);}
void	ChaosLabel::setNum(int num){QLabel::setNum(num);}
void	ChaosLabel::setNum(double num){QLabel::setNum(num);}
void	ChaosLabel::setPicture(const QPicture & picture){QLabel::picture();}
void	ChaosLabel::setPixmap(const QPixmap &pixmap){QLabel::setPixmap(pixmap);}
void	ChaosLabel::setText(const QString &string){
    if(text().compare(string) == 0) return;
    QLabel::setText(string);
    emit valueChanged(nodeUniqueID(),
                      string);
}
