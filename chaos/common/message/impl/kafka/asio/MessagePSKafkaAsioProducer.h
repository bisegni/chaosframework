/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_ASIO_PRODUCER_H__
#define __MESSAGE_PS_KAFKA_ASIO_PRODUCER_H__
#include <chaos/common/message/MessagePSProducer.h>
#include <boost/asio.hpp>
#include "libkafka_asio/libkafka_asio.h"

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

class MessagePSKafkaAsioProducer : public chaos::common::message::MessagePSProducer {
 protected:
  uint64_t    counter;
  ::libkafka_asio::Connection::Configuration configuration;
  ::libkafka_asio::Connection * connection;

  bool        running;
  boost::asio::io_service ios;
  boost::thread th;
  void poll();
  
 public:
  MessagePSKafkaAsioProducer();
  MessagePSKafkaAsioProducer(const std::string& k);
  ~MessagePSKafkaAsioProducer();
  int pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key);
  int pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key);

  void addServer(const std::string& url);

  int  applyConfiguration();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif