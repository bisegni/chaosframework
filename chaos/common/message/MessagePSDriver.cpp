#include "MessagePSDriver.h"
#ifdef KAFKA_RDK_ENABLE
#include "impl/kafka/rdk/MessagePSKafkaProducer.h"
#include "impl/kafka/rdk/MessagePSRDKafkaConsumer.h"

#endif
#ifdef KAFKA_ASIO_ENABLE
#include "impl/kafka/asio/MessagePSKafkaAsioProducer.h"
#include "impl/kafka/asio/MessagePSKafkaAsioConsumer.h"

#endif
#include <chaos/common/global.h>

namespace chaos {
    namespace common {
        namespace message {
        
    producer_uptr_t MessagePSDriver::getProducerDriver(const std::string&drvname,const std::string& k){

#ifdef KAFKA_RDK_ENABLE

                if(drvname=="RDK" || drvname=="rdk"){
                    return producer_uptr_t(new kafka::rdk::MessagePSKafkaProducer());
                }
#endif
#ifdef KAFKA_ASIO_ENABLE
                if(drvname=="ASIO"||drvname=="asio"){
                    return  producer_uptr_t(new kafka::asio::MessagePSKafkaAsioProducer());
                }
#endif

        throw chaos::CException(-5,"cannot find a producer driver for:"+drvname,__PRETTY_FUNCTION__);

                
    }
     consumer_uptr_t MessagePSDriver::getConsumerDriver(const std::string&drvname,const std::string& gid,const std::string& k){
         
   #ifdef KAFKA_RDK_ENABLE

                if(drvname=="RDK" || drvname=="rdk"){
                    return  consumer_uptr_t(new kafka::rdk::MessagePSRDKafkaConsumer(gid,k));
                }
    #endif
    #ifdef KAFKA_ASIO_ENABLE
                if(drvname=="ASIO"||drvname=="asio"){
                    return consumer_uptr_t(new kafka::asio::MessagePSKafkaAsioConsumer(gid,k));
                }
    #endif

            throw chaos::CException(-5,"cannot find a consumert driver for:"+drvname,__PRETTY_FUNCTION__);
                

     }


        }
        }
        }

