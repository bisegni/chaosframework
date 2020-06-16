#include "MessagePSProducer.h"
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
            }

            MessagePSProducer::MessagePSProducer(const std::string& clientid,const std::string& k):MessagePublishSubscribeBase(clientid),defkey(k){
                

            };
           

            MessagePSProducer::~MessagePSProducer(){

            }
         int MessagePSProducer::pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum){
             return 0;
         }
        
        int MessagePSProducer::pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum){

            return 0;
        }
        int MessagePSProducer::flush(const int timeout){
            return 0;

        }

              
        }}}