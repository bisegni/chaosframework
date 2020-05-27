#include "MessagePublishSubscribeBase.h"

namespace chaos {
    namespace common {
        namespace message {
                void MessagePublishSubscribeBase::addServer(const std::string&url){
                    servers.insert(url);
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