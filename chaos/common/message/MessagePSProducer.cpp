#include "MessagePSProducer.h"
#ifdef KAFKA_RDK_ENABLE
#include "impl/kafka/rdk/MessagePSKafkaProducer.h"
#endif
#ifdef KAFKA_ASIO_ENABLE
#include "impl/kafka/rdk/MessagePSKafkaAsioProducer.h"
#endif

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

                if(clientid=="ASIO"){
                    impl = new kafka::asio::MessagePSKafkaAsioProducer();
                }
                #endif

            };
            MessagePSProducer::~MessagePSProducer(){
                if(impl){delete impl;}

            }
         int MessagePSProducer::pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key){
             return ((MessagePSProducer*)impl)->pushMsgAsync(data,key);
         }
        void MessagePSProducer::addServer(const std::string&url){
            return impl->addServer(url);
        }

         int MessagePSProducer::applyConfiguration(){
             return ((MessagePSProducer*)impl)->applyConfiguration();
         }
        int MessagePSProducer::pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key){
            return ((MessagePSProducer*)impl)->pushMsgAsync(data,key);
        }
              
        }}}