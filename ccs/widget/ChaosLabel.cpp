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
    if(p_track_status == track_status) return;
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
        if(!ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                      20,
                                                                                      &healt_status_handler)) {
            return -2;
        }
        if(!ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                      20,
                                                                                      &healt_heartbeat_handler)) {
            return -3;
        }
    }

    return 0;
}

int ChaosLabel::stopMonitoring() {
    if(!monitoring) return -1;
    monitoring = false;
    if(!ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                     20,
                                                                                     &healt_heartbeat_handler)) {
        return -2;
    }
    if(!ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                     20,
                                                                                     &healt_status_handler)) {
        return -3;
    }
    return 0;
}

void ChaosLabel::valueUpdated(const QString& node_uid,
                              const QString& attribute_name,
                              const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0) {
        uint64_t received_ts = attribute_value.toLongLong();
        uint64_t time_diff = last_recevied_ts - received_ts;
        if(time_diff > 0) {
            setStyleSheet("QLabel { color : #4EB66B; }");
        } else {
            if(++zero_diff_count > 3) {
                //timeouted
                 setStyleSheet("QLabel { color : #E65566; }");
            } else {
                //in this case we do nothing perhaps we can to fast to check
            }
        }
        last_recevied_ts = received_ts;
    }else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
        //write the value
        setToolTip(attribute_value.toString());
        if(labelValueShowTrackStatus())
            setText(attribute_value.toString());
    }
}

void ChaosLabel::valueNotFound(const QString& node_uid,
                               const QString& attribute_name) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0) {
        last_recevied_ts = zero_diff_count = 0;
        setStyleSheet("QLabel { color : gray; }");
    }
}

void ChaosLabel::valueUpdated(const QString& node_uid,
                              const QString& attribute_name,
                              uint64_t timestamp,
                              const QVariant& attribute_value) {
    //write the value
    setText(attribute_value.toString());
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
