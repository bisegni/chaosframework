#ifndef ccs_HealthHeartbeatHandler
#define ccs_HealthHeartbeatHandler
#include "../AbstractAttributeHandler.h"

class HealthHeartbeatHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::handler::HealtHeartBeatAttributeHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value){
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<qulonglong>((uint64_t)value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute){
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    HealthHeartbeatHandler(bool event_on_change = false):
    AbstractAttributeHandler(this),
    HealtHeartBeatAttributeHandler(event_on_change){}
};
#endif // HEALTHEARTBEATHANDLER
