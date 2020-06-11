#include "MessagePSProducer.h"
#ifdef KAFKA_RDK_ENABLE
#include "impl/kafka/rdk/MessagePSKafkaProducer.h"
#endif
#ifdef KAFKA_ASIO_ENABLE
#include "impl/kafka/asio/MessagePSKafkaAsioProducer.h"
#endif
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSProducer)
#define MRDDBG_ DBG_LOG(MessagePSProducer)
#define MRDERR_ ERR_LOG(MessagePSProducer)

namespace chaos {
    namespace common {
        namespace message {

         /*   MessagePSProducer::MessagePSProducer(const std::string& clientid):MessagePublishSubscribeBase(clientid){
                
            };*/
            MessagePSProducer::MessagePSProducer():MessagePublishSubscribeBase(""),defkey(""){
                impl=NULL;
            }

            MessagePSProducer::MessagePSProducer(const std::string& clientid,const std::string& k):MessagePublishSubscribeBase(clientid),defkey(k){
                impl = NULL;
                #ifdef KAFKA_RDK_ENABLE

                if(clientid=="RDK" || clientid=="rdk"){
                    impl = new kafka::rdk::MessagePSKafkaProducer();
                }
                #endif
                #ifdef KAFKA_ASIO_ENABLE
                if(clientid=="ASIO"||clientid=="asio"){
                    impl = new kafka::asio::MessagePSKafkaAsioProducer();
                }
                #endif

                if(impl==NULL){
                    throw chaos::CException(-5,"cannot find a Kafka driver for:"+clientid,__PRETTY_FUNCTION__);
                }

            };
           

            MessagePSProducer::~MessagePSProducer(){
                if(impl){delete impl;}

            }
         int MessagePSProducer::pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum){
             stats.counter++;
             return ((MessagePSProducer*)impl)->pushMsgAsync(data,key,pnum);
         }
        
        int MessagePSProducer::pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum){
            stats.counter++;

            return ((MessagePSProducer*)impl)->pushMsgAsync(data,key,pnum);
        }

          int MessagePSProducer::deleteKey(const std::string&key){

            return ((MessagePSProducer*)impl)->deleteKey(key);
        }     
        }}}