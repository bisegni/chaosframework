/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_PRODUCER_H__
#define __MESSAGE_PS_PRODUCER_H__
#include "MessagePublishSubscribeBase.h"

namespace chaos {
    namespace common {
        namespace message {

            class MessagePSProducer: public MessagePublishSubscribeBase {

                protected:
                std::vector<std::string> servers;
                std::string client_id;
                std::string defkey;
                public:

                MessagePSProducer(const std::string& clientid):MessagePublishSubscribeBase(clientid){};
                MessagePSProducer(const std::string& clientid,const std::string& k):MessagePublishSubscribeBase(clientid),defkey(k){};

                virtual int pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key)=0;
                virtual int pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key)=0;
                inline int pushMsg(const chaos::common::data::CDataWrapper&data){if(defkey!="") return pushMsg(data,defkey);return -1;}
                inline int pushMsgAsync(const chaos::common::data::CDataWrapper&data){if(defkey!="") return pushMsgAsync(data,defkey);return -1;}


            };
        }
        }
        }
#endif