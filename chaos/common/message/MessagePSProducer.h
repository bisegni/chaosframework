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
                std::string client_id;
                std::string defkey;
                public:
                MessagePSProducer();

                MessagePSProducer(const std::string& clientid,const std::string& k="");
                virtual ~MessagePSProducer();
                virtual int pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum=0);
                virtual int pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum=0);
                inline int pushMsg(const chaos::common::data::CDataWrapper&data,const int32_t pnum=0){if(defkey!="") return pushMsg(data,defkey,pnum);return -1;}
                inline int pushMsgAsync(const chaos::common::data::CDataWrapper&data,const int32_t pnum=0){if(defkey!="") return pushMsgAsync(data,defkey,pnum);return -1;}
                virtual int deleteKey(const std::string&key);

            };
        }
        }
        }
#endif