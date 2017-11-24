#ifndef MONITORBINARYATTRIBUTEHANDLER
#define MONITORBINARYATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"
#include "../../data/ChaosByteArray.h"
#include <QSharedPointer>

class MonitorBinaryAttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBinaryHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const ChaosSharedPtr<chaos::common::data::SerializationBuffer>& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<QSharedPointer<ChaosByteArray> >(QSharedPointer<ChaosByteArray>(new ChaosByteArray(value))));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    MonitorBinaryAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBinaryHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORBINARYATTRIBUTEHANDLER

