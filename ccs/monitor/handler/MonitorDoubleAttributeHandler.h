#ifndef MONITORDOUBLEATTRIBUTEHANDLER
#define MONITORDOUBLEATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorDoubleAttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeDoubleHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const double value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<double>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    MonitorDoubleAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeDoubleHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORDOUBLEATTRIBUTEHANDLER

