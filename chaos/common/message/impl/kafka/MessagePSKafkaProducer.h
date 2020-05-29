/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_PRODUCER_H__
#define __MESSAGE_PS_PRODUCER_H__
#include "MessagePSRDKafka.h"

namespace chaos {
    namespace common {
        namespace message {
            class MessagePSKafkaProducer: public MessagePSRDKafka {

                protected:
                rd_kafka_t* rk;
                std::string client_id;
                std::string defkey;
                bool running;
                public:

                MessagePSKafkaProducer(const std::string& clientid);
                MessagePSKafkaProducer(const std::string& clientid,const std::string& k);
                ~MessagePSKafkaProducer();
                int pushMsgAsync(const chaos::common::data::CDataWrapper&data,const std::string&key);
                int pushMsg(const chaos::common::data::CDataWrapper&data,const std::string&key);
                

            
                int applyConfiguration();
                void poll();



            };
        }
        }
        }
#endif