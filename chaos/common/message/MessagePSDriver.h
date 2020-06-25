#ifndef __MESSAGE_PS_DRIVER__
#define __MESSAGE_PS_DRIVER__
#include "MessagePSProducer.h"
#include "MessagePSConsumer.h"

namespace chaos {
    namespace common {
        namespace message {
        typedef ChaosSharedPtr<MessagePSConsumer> consumer_uptr_t;
        typedef ChaosSharedPtr<MessagePSProducer> producer_uptr_t;

class MessagePSDriver {

    static boost::mutex io;
    static std::map<std::string,producer_uptr_t> producer_drv_m;
    static std::map<std::string,consumer_uptr_t> consumer_drv_m;

    public:
    static producer_uptr_t getProducerDriver(const std::string&drvname,const std::string& k="");
    static consumer_uptr_t getConsumerDriver(const std::string&drvname,const std::string& gid,const std::string& k="");


};
        }
        }
        }
#endif
