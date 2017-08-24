#ifndef MONITORINT32ATTRIBUTEHANDLER
#define MONITORINT32ATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"
#include <QDebug>

#include <chaos_metadata_service_client/monitor_system/monitor_system.h>

class MonitorInt32AttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt32Handler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int32_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<int32_t>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    MonitorInt32AttributeHandler(const QString& attribute_name,
                                 bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt32Handler(attribute_name.toStdString(),
                                                                                        event_on_change){}
};

#endif // MONITORINT32ATTRIBUTEHANDLER

