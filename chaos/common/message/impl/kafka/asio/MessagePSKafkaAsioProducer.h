/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PS_KAFKA_PRODUCER_H__
#define __MESSAGE_PS_KAFKA_PRODUCER_H__
#include <chaos/common/message/MessagePSProducer.h>
#include <boost/asio.hpp>
#include "libkafka_asio/libkafka_asio.h"

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

class MMessagePSKafkaAsioProducer : public chaos::common::message::MessagePSProducer {
 protected:
  uint64_t    counter;
  ::libkafka_asio::Connection::Configuration configuration;
  ::libkafka_asio::Connection * connection;

  bool        running;
  boost::asio::io_service ios;

 public:
  MMessagePSKafkaAsioProducer();
  MMessagePSKafkaAsioProducer(const std::string& k);
  ~MMessagePSKafkaAsioProducer();
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