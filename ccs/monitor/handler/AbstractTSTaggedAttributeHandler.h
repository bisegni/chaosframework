#ifndef ABSTRACTTSTAGGEDATTRIBUTEHANDLER
#define ABSTRACTTSTAGGEDATTRIBUTEHANDLER
#include <QObject>
#include <QVariant>
#include <QDebug>
#include <QString>
#include <chaos_metadata_service_client/monitor_system/monitor_system.h>

class AbstractTSTaggedAttributeHandler:
        public QObject {
    Q_OBJECT
public:
    explicit AbstractTSTaggedAttributeHandler(chaos::metadata_service_client::monitor_system::AbstractQuantumTSTaggedAttributeHandler *_quantum_attribute_handler):
        QObject(NULL),
        quantum_attribute_handler(_quantum_attribute_handler){}

    chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *getQuantumAttributeHandler(){
        return quantum_attribute_handler;
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& attribute_name,
                      const uint64_t timestamp,
                      const QVariant& attribute_value);
    void valueNotFound(const QString& key,
                       const QString& attribute_name);
protected:
chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *quantum_attribute_handler;
};

#endif // ABSTRACTTSTAGGEDATTRIBUTEHANDLER

