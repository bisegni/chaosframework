#ifndef ABSTRACTATTRIBUTEHANDLER
#define ABSTRACTATTRIBUTEHANDLER
#include <QObject>
#include <QVariant>
#include <QDebug>
#include <QString>

class AbstractAttributeHandler:
        public QObject {
    Q_OBJECT
public:
    explicit AbstractAttributeHandler(chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *_quantum_attribute_handler):
        QObject(NULL),
        quantum_attribute_handler(_quantum_attribute_handler){}

    chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *getQuantumAttributeHandler(){
        return quantum_attribute_handler;
    }

signals:
    void valueUpdated(const QString& key,
                      const QString& attribute_name,
                      const QVariant& attribute_value);

    void valueNotFound(const QString& key,
                       const QString& attribute_name);
protected:
    chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *quantum_attribute_handler;
};
#endif // ABSTRACTHEALTHANDLER

