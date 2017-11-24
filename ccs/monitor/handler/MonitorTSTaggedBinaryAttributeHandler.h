#ifndef MonitorTSTaggedBinaryAttributeHandler_h
#define MonitorTSTaggedBinaryAttributeHandler_h

#include "AbstractTSTaggedAttributeHandler.h"
#include "../../data/ChaosByteArray.h"
#include <QObject>
#include <QSharedPointer>


class MonitorTSTaggedBinaryAttributeHandler:
        public AbstractTSTaggedAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumTSTaggedBinaryAttributeHandler {
    Q_OBJECT

protected:
    void consumeTSTaggedValue(const std::string& key,
                              const std::string& attribute,
                              uint64_t timestamp,
                              const ChaosSharedPtr<chaos::common::data::SerializationBuffer>& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          timestamp,
                          QVariant::fromValue<QSharedPointer<ChaosByteArray> >(QSharedPointer<ChaosByteArray>(new ChaosByteArray(value))));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                          QString::fromStdString(attribute));
    }

public:
    MonitorTSTaggedBinaryAttributeHandler(const QString& attribute_name,
                                          const QString& ts_attribute_name,
                                          bool event_on_change = false):

        AbstractTSTaggedAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumTSTaggedBinaryAttributeHandler(attribute_name.toStdString(),
                                                                                              ts_attribute_name.toStdString(),
                                                                                              event_on_change){}
};
#endif // MonitorTSTaggedBinaryAttributeHandler_h

