#ifndef SYSTEMCONTROLUNIT
#define SYSTEMCONTROLUNIT
#include "../AbstractAttributeHandler.h"
#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>
class SystemControlUnitRunScheduleDelay:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeInt64Handler
{
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<int64_t>(value));
    }
public:
    SystemControlUnitRunScheduleDelay(bool event_on_change = false):
    QuantumKeyAttributeInt64Handler(std::string(chaos::ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY), event_on_value_change){}
};
#endif // SYSTEMCONTROLUNIT

