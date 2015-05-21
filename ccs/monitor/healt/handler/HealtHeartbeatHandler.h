#ifndef HEALTHEARTBEATHANDLER
#define HEALTHEARTBEATHANDLER
#include "AbstractHealtHandler.h"

class HealthHartbeatHandler:
        public AbstractHealtHandler,
        public chaos::metadata_service_client::monitor_system::handler::HealtHeartBeatAttributeHandler
{
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
};
#endif // HEALTHEARTBEATHANDLER

