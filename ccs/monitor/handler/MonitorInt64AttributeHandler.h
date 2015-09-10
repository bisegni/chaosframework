#ifndef MONITORINT64ATTRIBUTEHANDLER
#define MONITORINT64ATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorInt64AttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt64Handler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<int64_t>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    MonitorInt64AttributeHandler(const QString& attribute_name,
                                 bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt64Handler(attribute_name.toStdString(),
                                                                                        event_on_change){}
};

#endif // MONITORINT64ATTRIBUTEHANDLER

