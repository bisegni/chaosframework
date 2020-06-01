#include "MessagePublishSubscribeBase.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePublishSubscribeBase)
#define MRDDBG_ DBG_LOG(MessagePublishSubscribeBase)
#define MRDERR_ ERR_LOG(MessagePublishSubscribeBase)

namespace chaos {
    namespace common {
        namespace message {

            
                void MessagePublishSubscribeBase::addServer(const std::string&url){
                    servers.insert(url);
                    MRDDBG_<<"["<<servers.size()<<"] adding server:"<<url;
                }

                int MessagePublishSubscribeBase::setOption(const std::string&key,const std::string& value){
                    return 0;
                }
                int MessagePublishSubscribeBase::addHandler(eventTypes ev,msgpshandler cb){
                    handlers[ev]=cb;
                    return 0;
                }



        }
        }
        }