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

   boost::mutex MessagePSDriver::io;
   std::map<std::string,producer_uptr_t> MessagePSDriver::producer_drv_m;
   std::map<std::string,consumer_uptr_t> MessagePSDriver::consumer_drv_m;
      
    producer_uptr_t MessagePSDriver::getProducerDriver(const std::string&drvname,const std::string& k){
    boost::mutex::scoped_lock ll(io);
    std::map<std::string,producer_uptr_t>::iterator i=producer_drv_m.find(drvname);
    if(i!=producer_drv_m.end()){
        return i->second;
    }
producer_uptr_t ret;
#ifdef KAFKA_RDK_ENABLE

                if(drvname=="KAFKA-RDK" || drvname=="kafka-rdk"){
                    ret.reset(new kafka::rdk::MessagePSKafkaProducer());
                    producer_drv_m["kafka-rdk"]=ret;
                    return ret;
                }
#endif
#ifdef KAFKA_ASIO_ENABLE
                if(drvname=="KAFKA-ASIO"||drvname=="kafka-asio"){
                    ret.reset(new kafka::asio::MessagePSKafkaAsioProducer());
                    producer_drv_m["kafka-asio"]=ret;

                    return  ret;
                }
#endif

        throw chaos::CException(-5,"cannot find a producer driver for:"+drvname,__PRETTY_FUNCTION__);

                
    }
     consumer_uptr_t MessagePSDriver::getConsumerDriver(const std::string&drvname,const std::string& gid,const std::string& k){
             std::map<std::string,consumer_uptr_t>::iterator i=consumer_drv_m.find(drvname);

         consumer_uptr_t ret;
          if(i!=consumer_drv_m.end()){
                return i->second;
        }
   #ifdef KAFKA_RDK_ENABLE

                if(drvname=="KAFKA-RDK" || drvname=="kafka-rdk"){
                    ret.reset(new kafka::rdk::MessagePSRDKafkaConsumer(gid,k));
                    consumer_drv_m["kafka-rdk"]=ret;
                    return  ret;
                }
    #endif
    #ifdef KAFKA_ASIO_ENABLE
                if(drvname=="KAFKA-ASIO"||drvname=="kafka-asio"){
                    ret.reset(new kafka::rdk::MessagePSRDKafkaConsumer(gid,k));
                    consumer_drv_m["kafka-asio"]=ret;
                    return  ret;
                }
    #endif

            throw chaos::CException(-5,"cannot find a consumert driver for:"+drvname,__PRETTY_FUNCTION__);
                

     }


        }
        }
        }

