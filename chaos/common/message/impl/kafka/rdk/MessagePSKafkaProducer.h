/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_PRODUCER_H__
#define __MESSAGE_PS_KAFKA_PRODUCER_H__
#include <chaos/common/message/MessagePSProducer.h>
#include "MessagePSRDKafka.h"
namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {

class MessagePSKafkaProducer : public MessagePSRDKafka, public chaos::common::message::MessagePSProducer {
 protected:
  uint64_t    counter;
  rd_kafka_t* rk;
  bool        running;

 public:
  MessagePSKafkaProducer();
  MessagePSKafkaProducer(const std::string& k);
  ~MessagePSKafkaProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key);

  void addServer(const std::string& url);

  int  applyConfiguration();
  void poll();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif