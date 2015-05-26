#ifndef HEALTSTATUSHANDLER_H
#define HEALTSTATUSHANDLER_H
#include "AbstractAttributeHandler.h"

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
                          QVariant::fromValue<QString>(QString::fromStdString(value)));
    }
public:
    HealtStatusHandler(bool event_on_change = false):
    HealtStatusAttributeHandler(event_on_change){}
};
#endif // HEALTSTATUSHANDLER_H
