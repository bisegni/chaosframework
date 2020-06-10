/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_RDKKAFKA_PRODUCER_H__
#define __MESSAGE_PS_RDKKAFKA_PRODUCER_H__
#include <chaos/common/message/MessagePSProducer.h>
#include "MessagePSRDKafka.h"
namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {

class MessagePSKafkaProducer : public MessagePSRDKafka, public chaos::common::message::MessagePSProducer {
 protected:
  rd_kafka_t* rk;
  bool        running;

 public:
  MessagePSKafkaProducer();
  MessagePSKafkaProducer(const std::string& k);
  ~MessagePSKafkaProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum=0);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum=0);

  int  applyConfiguration();
  void poll();
  int deleteKey(const std::string&key);
  int waitCompletion(const uint32_t timeout_ms=5000);

};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif