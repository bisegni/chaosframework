#ifndef MONITORSTRINGATTRIBUTEHANDLER
#define MONITORSTRINGATTRIBUTEHANDLER

#include <QObject>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorStringAttributeHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeStringHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const std::string& value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QString::fromStdString(value));
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& name,
                      const QString& value);
public:
    MonitorStringAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeStringHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORSTRINGATTRIBUTEHANDLER

