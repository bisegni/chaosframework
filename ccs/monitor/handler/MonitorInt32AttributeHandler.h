#ifndef MONITORINT32ATTRIBUTEHANDLER
#define MONITORINT32ATTRIBUTEHANDLER

#include <QObject>
#include <QDebug>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorInt32AttributeHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt32Handler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int32_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          value);
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& name,
                      const int32_t value);
public:
    MonitorInt32AttributeHandler(const QString& attribute_name,
                                 bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt32Handler(attribute_name.toStdString(),
                                                                                        event_on_change){}
};

#endif // MONITORINT32ATTRIBUTEHANDLER

