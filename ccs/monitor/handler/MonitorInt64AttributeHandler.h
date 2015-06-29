#ifndef MONITORINT64ATTRIBUTEHANDLER
#define MONITORINT64ATTRIBUTEHANDLER

#include <QObject>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorInt64AttributeHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt64Handler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          value);
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& name,
                      const int64_t value);
public:
    MonitorInt64AttributeHandler(const QString& attribute_name,
                                 bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt64Handler(attribute_name.toStdString(),
                                                                                        event_on_change){}
};

#endif // MONITORINT64ATTRIBUTEHANDLER

