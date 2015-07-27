#ifndef MONITORBOOLATTRIBUTEHANDLER
#define MONITORBOOLATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorBoolAttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBoolHandler {
    Q_OBJECT

protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const bool value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<bool>(value));
    }

public:
    MonitorBoolAttributeHandler(const QString& attribute_name,
                                bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBoolHandler(attribute_name.toStdString(),
                                                                                       event_on_change){}
};

#endif // MONITORBOOLATTRIBUTEHANDLER

