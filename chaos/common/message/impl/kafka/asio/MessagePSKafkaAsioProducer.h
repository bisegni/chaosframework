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

class MessagePSKafkaAsioProducer : public chaos::common::message::MessagePSProducer,MessagePSKafkaAsio {
 protected:
  
  void HandleMeta(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::MetadataResponse::OptionalType& response);
  void HandleRequest(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::ProduceResponse::OptionalType& response);

 public:
  MessagePSKafkaAsioProducer();
  MessagePSKafkaAsioProducer(const std::string& k);
  ~MessagePSKafkaAsioProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum);
  int deleteKey(const std::string& key);
  int waitCompletion(const uint32_t timeout_ms){
    return MessagePSKafkaAsio::waitCompletion(timeout_ms);
  }
  int  applyConfiguration();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif