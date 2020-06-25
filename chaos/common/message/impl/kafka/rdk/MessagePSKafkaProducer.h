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
  bool        running;
  public:

  void HandleRequest(rd_kafka_t*               rk,
                      const rd_kafka_message_t* rkmessage);
 
  MessagePSKafkaProducer();
  MessagePSKafkaProducer(const std::string& k);
  ~MessagePSKafkaProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum=0);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum=0);

  int  applyConfiguration();
  void poll();

  int deleteKey(const std::string&key);
  int flush(const int timeo=10000);

};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif