#ifndef MonitorTSTaggedInt64AttributeHandler_h
#define MonitorTSTaggedInt64AttributeHandler_h
#include <QObject>
#include "AbstractTSTaggedAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorTSTaggedInt64AttributeHandler:
        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedInt64AttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const int64_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QVariant::fromValue<int64_t>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedInt64AttributeHandler(const QString& attribute_name,
                                         const QString& ts_attribute_name,
                                         bool event_on_change = false):
        AbstractTSTaggedAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedInt64AttributeHandler(attribute_name.toStdString(),
                                                                                             ts_attribute_name.toStdString(),
                                                                                             event_on_change){}
};
#endif // MonitorTSTaggedInt64AttributeHandler_h

