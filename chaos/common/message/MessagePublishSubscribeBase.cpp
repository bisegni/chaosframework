#include "MessagePublishSubscribeBase.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePublishSubscribeBase)
#define MRDDBG_ DBG_LOG(MessagePublishSubscribeBase)
#define MRDERR_ ERR_LOG(MessagePublishSubscribeBase)

namespace chaos {
    namespace common {
        namespace message {

            MessagePublishSubscribeBase::~MessagePublishSubscribeBase(){
                stop();
            
            }
                void MessagePublishSubscribeBase::addServer(const std::string&url){
                    MRDDBG_<<"["<<servers.size()<<"] adding server:"<<url;
                    servers.insert(url);
                }

                int MessagePublishSubscribeBase::setOption(const std::string&key,const std::string& value){
                    MRDDBG_<<"NOT IMPLEMENTED";

                    return 0;
                }
                int MessagePublishSubscribeBase::addHandler(eventTypes ev,msgHandler cb){
                    handlers[ev]=cb;
                    return 0;
                }


        /* int MessagePublishSubscribeBase::applyConfiguration(){
             if(impl==NULL){
                MRDERR_<<"NOT a valid implementation";
                return -5;

            }
             return impl->applyConfiguration();
         }*/

          int MessagePublishSubscribeBase::applyConfiguration(){
                MRDDBG_<<"NOT IMPLEMENTED";

            return 0;
            }
         int MessagePublishSubscribeBase::waitCompletion(const uint32_t timeout_ms){
                boost::unique_lock<boost::mutex> guard(mutex_cond);
                MRDDBG_<<"wating operation";
                if(data_ready) return stats.last_err;
                if(!cond.timed_wait(guard,boost::posix_time::milliseconds(timeout_ms))){
                MRDERR_<<"Timeout";
                return -100;
                }

                return stats.last_err; 
            }

        int MessagePublishSubscribeBase::deleteKey(const std::string& key){
           MRDDBG_<<"NOT IMPLEMENTED";

            return 0;
         
        }
        int MessagePublishSubscribeBase::setMaxMsgSize(const int size){
           MRDDBG_<<"NOT IMPLEMENTED";

            return 0;
          
        }
         void MessagePublishSubscribeBase::thfunc(){
                running =true;
                MRDDBG_<<"Start polling";
                while(running){
                    poll();
                }
                MRDDBG_<<"End polling";
            }
    
         int MessagePublishSubscribeBase::createKey(const std::string& key){
              MRDDBG_<<"NOT IMPLEMENTED";

            return 0;

         }
         void MessagePublishSubscribeBase::poll(){
             MRDDBG_<<"NOT IMPLEMENTED";

         }

         void MessagePublishSubscribeBase::start(){
             running=true;
             boost::thread(&MessagePublishSubscribeBase::thfunc,this);

         }
        void MessagePublishSubscribeBase::stop(){
            running=false;
            th.join();
        }
                

        }
        }
        }