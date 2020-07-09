/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_RDK_CONSUMER_H__
#define __MESSAGE_PS_KAFKA_RDK_CONSUMER_H__
#include <chaos/common/message/MessagePSConsumer.h>
#include "MessagePSRDKafka.h"
namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {

class MessagePSRDKafkaConsumer : public MessagePSRDKafka,public chaos::common::message::MessagePSConsumer {
 public:
  
  MessagePSRDKafkaConsumer(const std::string& gid,const std::string& defkey);
  ~MessagePSRDKafkaConsumer();
  int applyConfiguration();
  int getMsgAsync(const std::string&key,const int32_t pnum=0);
  int getMsgAsync(const std::string&key,uint32_t offset,const int32_t pnum=0);
  ele_uptr_t getMsg(int timeo);
  void poll();
  int subscribe(const std::string& key);


};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif