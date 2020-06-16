#ifndef __MESSAGE_PS_DRIVER__
#define __MESSAGE_PS_DRIVER__
#include "MessagePSProducer.h"
#include "MessagePSConsumer.h"

namespace chaos {
    namespace common {
        namespace message {
        typedef ChaosUniquePtr<MessagePSConsumer> consumer_uptr_t;
        typedef ChaosUniquePtr<MessagePSProducer> producer_uptr_t;

class MessagePSDriver {


    public:
    static producer_uptr_t getProducerDriver(const std::string&drvname,const std::string& k="");
    static consumer_uptr_t getConsumerDriver(const std::string&drvname,const std::string& gid,const std::string& k="");


};
        }
        }
        }
#endif
