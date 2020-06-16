/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_ASIO_CONSUMER_H__
#define __MESSAGE_PS_KAFKA_ASIO_CONSUMER_H__
#include <chaos/common/message/MessagePSConsumer.h>
#include <boost/asio.hpp>
#include "libkafka_asio/libkafka_asio.h"
#include "MessagePSKafkaAsio.h"
namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

class MessagePSKafkaAsioConsumer : public chaos::common::message::MessagePSConsumer,public MessagePSKafkaAsio {
 protected:

void HandleRequest(const ::libkafka_asio::Connection::ErrorCodeType& err,
                   const ::libkafka_asio::FetchResponse::OptionalType& response);
 public:
  
  MessagePSKafkaAsioConsumer(const std::string& gid,const std::string& k);
  ~MessagePSKafkaAsioConsumer();

  int getMsgAsync(const std::string&key,const int32_t pnum=0);
  int getMsgAsync(const std::string&key,uint32_t offset,const int32_t pnum=0);
  int msgInQueue(){return que_elem;}
  ele_uptr_t getMsg(int timeo);

};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif