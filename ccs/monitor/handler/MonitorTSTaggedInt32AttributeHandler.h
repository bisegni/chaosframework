#ifndef MONITORTSTAGGEDINT32ATTRIBUTEHANDLER
#define MONITORTSTAGGEDINT32ATTRIBUTEHANDLER
#include <QObject>
#include "AbstractTSTaggedAttributeHandler.h"

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorTSTaggedInt32AttributeHandler:
        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedInt32AttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const int32_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QVariant::fromValue<int32_t>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedInt32AttributeHandler(const QString& attribute_name,
                                         const QString& ts_attribute_name,
                                         bool event_on_change = false):
        AbstractTSTaggedAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedInt32AttributeHandler(attribute_name.toStdString(),
                                                                                            ts_attribute_name.toStdString(),
                                                                                            event_on_change){}
};
#endif // MONITORTSTAGGEDINT32ATTRIBUTEHANDLER

