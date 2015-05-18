#ifndef HEALTSTATUSHANDLER_H
#define HEALTSTATUSHANDLER_H
#include "AbstractHealtHandler.h"

class HealtStatusHandler:
        public AbstractHealtHandler,
        public chaos::metadata_service_client::monitor_system::handler::HealtStatusAttributeHandler
{
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const std::string& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(attribute),
                          QVariant::fromValue<QString>(QString::fromStdString(value)));
    }
};
#endif // HEALTSTATUSHANDLER_H
