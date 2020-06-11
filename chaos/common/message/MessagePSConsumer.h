/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_CONSUMER_H__
#define __MESSAGE_PS_CONSUMER_H__
#include "MessagePublishSubscribeBase.h"

namespace chaos {
    namespace common {
        namespace message {

            class MessagePSConsumer: public MessagePublishSubscribeBase {

                protected:

                msg_queue_t msgs;
                std::string client_id;
                std::string defkey;
                public:
                MessagePSConsumer();

                MessagePSConsumer(const std::string& clientid,const std::string& k="");
                virtual ~MessagePSConsumer();
                
                /**
                 * @brief Retrive a message with the given topic name
                 * 
                 * @param key ke
                 * @param pnum 
                 * @return int 
                 */
                virtual int getMsgAsync(const std::string&key,const int32_t pnum=0);
                virtual int getMsgAsync(const std::string&key,uint32_t offset,const int32_t pnum=0);
               
                int msgInQueue(){return msgs.size();}
                chaos::common::data::CDWUniquePtr getMsg(int index){if(index<msgs.size()){return MOVE(msgs[index]);}  return chaos::common::data::CDWUniquePtr();}

               // virtual int retriveMsg(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum=0);


            };
        }
        }
        }
#endif