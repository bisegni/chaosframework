#include "CLedIndicatorHealt.h"
#include <chaos/common/chaos_constants.h>

#include <QDateTime>
#include <QDebug>

#define RETRY_TIME_FOR_OFFLINE 6

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

CLedIndicatorHealt::CLedIndicatorHealt(QWidget *parent):
    LedIndicatorWidget(parent),
    no_ts(new QIcon(":/images/white_circle_indicator.png")),
    timeouted(new QIcon(":/images/red_circle_indicator.png")),
    alive(new QIcon(":/images/green_circle_indicator.png")),
    alive_state(Stopped),
    last_recevied_ts(0),
    zero_diff_count(0) {
    addState(0, no_ts);
    addState(1, timeouted);
    addState(2, alive);
    connect(&hb_health_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(valueUpdated(QString,QString,QVariant)));
    connect(&hb_health_handler,
            SIGNAL(valueNotFound(QString,QString)),
            SLOT(valueNotFound(QString,QString)));

}

CLedIndicatorHealt::~CLedIndicatorHealt() {
    stopMonitoring();
}

void CLedIndicatorHealt::setNodeUniqueID(const QString& node_uid) {
    p_node_uid = node_uid;
}

QString CLedIndicatorHealt::nodeUniqueID() {
    return p_node_uid;
}

int CLedIndicatorHealt::startMonitoring() {
    if(!ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                  10,
                                                                                  &hb_health_handler)) {
        return -1;
    }
    return 0;
}

int CLedIndicatorHealt::stopMonitoring() {
    if(!ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(nodeUniqueID().toStdString(),
                                                                                     10,
                                                                                     &hb_health_handler)) {
        return -1;
    }
    return 0;
}

void CLedIndicatorHealt::manageOnlineFlag(AliveState current_alive_state) {
    if(current_alive_state == alive_state) return;
    emit changedOnlineStatus(nodeUniqueID(),
                             alive_state = current_alive_state);
}

void CLedIndicatorHealt::valueUpdated(const QString& node_uid,
                                      const QString& attribute_name,
                                      const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP)==0) {
        uint64_t current_timestamp = attribute_value.toULongLong();
        uint64_t time_diff = current_timestamp - last_recevied_ts;
        if(time_diff > 0) {
            //in time
            setState(2);
            manageOnlineFlag(Online);
            zero_diff_count = 0;
        } else {
            if(++zero_diff_count > RETRY_TIME_FOR_OFFLINE) {
                //timeouted
                setState(1);
                manageOnlineFlag(Offline);
            } else {
                //in this case we do nothing perhaps we can to fast to check
            }
        }
        last_recevied_ts = current_timestamp;
    }
}

void CLedIndicatorHealt::valueNotFound(const QString& node_uid,
                                       const QString& attribute_name) {
    setState(0);
    manageOnlineFlag(Indeterminated);
    last_recevied_ts = zero_diff_count = 0;
}
