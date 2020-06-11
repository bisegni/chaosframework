/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PSKAFKASIO_H__
#define __MESSAGE_PSKAFKASIO_H__
#include <chaos/common/message/MessagePublishSubscribeBase.h>

#include "libkafka_asio/libkafka_asio.h"

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {
class MessagePSKafkaAsio {
 protected:
  ::libkafka_asio::Connection::Configuration configuration;
  ::libkafka_asio::Connection * connection;
  boost::asio::io_service ios;
  boost::thread th;
  bool        running;
  bool        data_ready;
  boost::mutex mutex_cond;
  boost::condition_variable cond;

void poll();

 public:
  MessagePSKafkaAsio();
  ~MessagePSKafkaAsio();

  int setOption(const std::string& key, const std::string& value);

  int init(std::set<std::string>& servers);
  int waitCompletion(const uint32_t timeout_ms=5000);

};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif