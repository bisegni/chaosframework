#ifndef MONITORBINARYATTRIBUTEHANDLER
#define MONITORBINARYATTRIBUTEHANDLER

#include <QObject>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorBinaryAttributeHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBinaryHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const boost::shared_ptr<chaos::common::data::SerializationBuffer>& buffer) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          buffer);
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& name,
                      const boost::shared_ptr<chaos::common::data::SerializationBuffer>& value);
public:
    MonitorBinaryAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBinaryHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORBINARYATTRIBUTEHANDLER

