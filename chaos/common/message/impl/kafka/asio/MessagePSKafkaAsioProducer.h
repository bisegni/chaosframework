/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_ASIO_PRODUCER_H__
#define __MESSAGE_PS_KAFKA_ASIO_PRODUCER_H__
#include <chaos/common/message/MessagePSProducer.h>
#include <boost/asio.hpp>
#include "libkafka_asio/libkafka_asio.h"
#include "MessagePSKafkaAsio.h"

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

class MessagePSKafkaAsioProducer : public chaos::common::message::MessagePSProducer,public MessagePSKafkaAsio {
 protected:
  
 public:
  MessagePSKafkaAsioProducer();
  MessagePSKafkaAsioProducer(const std::string& k);
  ~MessagePSKafkaAsioProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum);
  
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif