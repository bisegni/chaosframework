#ifndef MonitorTSTaggedDoubleAttributeHandler_h
#define MonitorTSTaggedDoubleAttributeHandler_h
#include <QObject>
#include "AbstractTSTaggedAttributeHandler.h"

#include <chaos_metadata_service_client/monitor_system/monitor_system.h>

class MonitorTSTaggedDoubleAttributeHandler:
        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedDoubleAttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const double value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QVariant::fromValue<double>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedDoubleAttributeHandler(const QString& attribute_name,
                                          const QString& ts_attribute_name,
                                          bool event_on_change = false):
        AbstractTSTaggedAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedDoubleAttributeHandler(attribute_name.toStdString(),
                                                                                              ts_attribute_name.toStdString(),
                                                                                              event_on_change){}
};
#endif // MonitorTSTaggedDoubleAttributeHandler_h

