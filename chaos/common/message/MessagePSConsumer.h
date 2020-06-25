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
            class MessagePSConsumer: public virtual MessagePublishSubscribeBase {

                protected:
                msg_queue_t msgs;
                std::string client_id;
                std::string defkey;
                std::string groupid;
                boost::atomic<uint32_t> que_elem;
                // subscribe list
                std::set<std::string> keylist;

                public:

                MessagePSConsumer(const std::string& clientid,const std::string& gid,const std::string& k="");
                ~MessagePSConsumer();
                
                /**
                 * @brief Retrive a message with the given topic name
                 * 
                 * @param key ke
                 * @param pnum 
                 * @return int 
                 */
                virtual int getMsgAsync(const std::string&key,const int32_t pnum=0);
                virtual int getMsgAsync(const std::string&key,uint32_t offset,const int32_t pnum=0);
               
                int msgInQueue(){return que_elem;}
                ele_uptr_t getMsg(int timeo=MSG_TIMEOUT_MS);
               // virtual int retriveMsg(const chaos::common::data::CDataWrapper&data,const std::string&key,const int32_t pnum=0);

                virtual int subscribe(const std::string& key);

            };
        }
        }
        }
#endif