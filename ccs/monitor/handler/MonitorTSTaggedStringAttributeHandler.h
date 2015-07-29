#ifndef MonitorTSTaggedStringAttributeHandler_h
#define MonitorTSTaggedStringAttributeHandler_h
#include <QObject>
#include "AbstractTSTaggedAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorTSTaggedStringAttributeHandler:
        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedStringAttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const std::string& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QString::fromStdString(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedStringAttributeHandler(const QString& attribute_name,
                                          const QString& ts_attribute_name,
                                          bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedStringAttributeHandler(attribute_name.toStdString(),
                                                                                              ts_attribute_name.toStdString(),
                                                                                              event_on_change){}
};
#endif // MonitorTSTaggedStringAttributeHandler_h

