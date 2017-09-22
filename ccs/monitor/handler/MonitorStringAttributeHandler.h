#ifndef MONITORSTRINGATTRIBUTEHANDLER
#define MONITORSTRINGATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"

#include <chaos_metadata_service_client/monitor_system/monitor_system.h>

class MonitorStringAttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeStringHandler {
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
    MonitorStringAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeStringHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORSTRINGATTRIBUTEHANDLER

