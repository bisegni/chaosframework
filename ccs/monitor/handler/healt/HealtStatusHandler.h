#ifndef ccs_HealtStatusHandler
#define ccs_HealtStatusHandler
#include "../AbstractAttributeHandler.h"

class HealtStatusHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::handler::HealtStatusAttributeHandler
{
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const std::string& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QString::fromStdString(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    HealtStatusHandler(bool event_on_change = false):
        AbstractAttributeHandler(this),
        HealtStatusAttributeHandler(event_on_change){}
};
#endif // HEALTSTATUSHANDLER_H
