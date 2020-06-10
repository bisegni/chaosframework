#include "MessagePSConsumer.h"
#ifdef KAFKA_RDK_ENABLE
//#include "impl/kafka/rdk/MessagePSKafkaConsumer.h"
#endif

#ifdef KAFKA_ASIO_ENABLE
#include "impl/kafka/asio/MessagePSKafkaAsioConsumer.h"
#endif

#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSConsumer)
#define MRDDBG_ DBG_LOG(MessagePSConsumer)
#define MRDERR_ ERR_LOG(MessagePSConsumer)

namespace chaos {
    namespace common {
        namespace message {

         /*   MessagePSConsumer::MessagePSConsumer(const std::string& clientid):MessagePublishSubscribeBase(clientid){
                
            };*/
            MessagePSConsumer::MessagePSConsumer():MessagePublishSubscribeBase(""),defkey(""){
                impl=NULL;
            }

            MessagePSConsumer::MessagePSConsumer(const std::string& clientid,const std::string& k):MessagePublishSubscribeBase(clientid),defkey(k){
                impl = NULL;
                #ifdef KAFKA_RDK_ENABLE

                if(clientid=="RDK" || clientid=="rdk"){
                   //x impl = new kafka::rdk::MessagePSKafkaConsumer();
                }
                #endif
                #ifdef KAFKA_ASIO_ENABLE
                if(clientid=="ASIO"||clientid=="asio"){
                    impl = new kafka::asio::MessagePSKafkaAsioConsumer();
                }
                #endif

                if(impl==NULL){
                    throw chaos::CException(-5,"cannot find a Kafka driver for:"+clientid,__PRETTY_FUNCTION__);
                }

            };
            MessagePSConsumer::~MessagePSConsumer(){
                if(impl){delete impl;}

            }
         int MessagePSConsumer::getMsgAsync(const std::string&key,const int32_t pnum){
             return ((MessagePSConsumer*)impl)->getMsgAsync(key,pnum);
         }

           int MessagePSConsumer::getMsgAsync(const std::string&key,uint32_t off,const int32_t pnum){
             return ((MessagePSConsumer*)impl)->getMsgAsync(key,off,pnum);
         }
       
         
       
              
        }}}    