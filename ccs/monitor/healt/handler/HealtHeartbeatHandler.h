#ifndef HEALTHEARTBEATHANDLER
#define HEALTHEARTBEATHANDLER
#include "AbstractAttributeHandler.h"

class HealthHartbeatHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::handler::HealtHeartBeatAttributeHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<qulonglong>((uint64_t)value));
    }

public:
    HealthHartbeatHandler(bool event_on_change = false):
    HealtHeartBeatAttributeHandler(event_on_change){}
};
#endif // HEALTHEARTBEATHANDLER

