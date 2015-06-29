#ifndef MONITORDOUBLEATTRIBUTEHANDLER
#define MONITORDOUBLEATTRIBUTEHANDLER

#include <QObject>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class MonitorDoubleAttributeHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeDoubleHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const double value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          value);
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& name,
                      const double value);
public:
    MonitorDoubleAttributeHandler(const QString& attribute_name,
                                  bool event_on_change = false):
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeDoubleHandler(attribute_name.toStdString(),
                                                                                         event_on_change){}
};

#endif // MONITORDOUBLEATTRIBUTEHANDLER

