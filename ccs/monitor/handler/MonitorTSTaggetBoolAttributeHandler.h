#ifndef MONITORTSTAGGETBOOLATTRIBUTEHANDLER
#define MONITORTSTAGGETBOOLATTRIBUTEHANDLER
#include <QObject>
#include "AbstractTSTaggedAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorTSTaggedBoolAttributeHandler:        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedBoolAttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const bool value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QVariant::fromValue<bool>(value));
    }

    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedBoolAttributeHandler(const QString& attribute_name,
                                        const QString& ts_attribute_name,
                                        bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedBoolAttributeHandler(attribute_name.toStdString(),
                                                                                            ts_attribute_name.toStdString(),
                                                                                            event_on_change){}
};
#endif // MONITORTSTAGGETBOOLATTRIBUTEHANDLER

