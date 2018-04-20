#ifndef ccs_SystemControlUnitRunScheduleDelay
#define ccs_SystemControlUnitRunScheduleDelay
#include "../AbstractAttributeHandler.h"
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
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    SystemControlUnitRunScheduleDelay(bool event_on_change):
        AbstractAttributeHandler(this),
        QuantumKeyAttributeInt64Handler(std::string(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY),
                                        event_on_change){}
};
#endif // SYSTEMCONTROLUNIT

