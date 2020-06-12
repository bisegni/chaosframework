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
class MessagePSKafkaAsio:public virtual MessagePublishSubscribeBase {
 protected:
  ::libkafka_asio::Connection::Configuration configuration;
  ::libkafka_asio::Connection * connection;
  boost::asio::io_service ios;
  boost::thread th;
  bool        running;
  bool        data_ready;
  boost::mutex mutex_cond;
  boost::condition_variable cond;

 void HandleMeta(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::MetadataResponse::OptionalType& response);
void HandleRequest(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::ProduceResponse::OptionalType& response);

 
void poll();

 public:
  MessagePSKafkaAsio():MessagePublishSubscribeBase("asio"),connection(NULL),running(false),data_ready(false) {
}

  ~MessagePSKafkaAsio();

  int setOption(const std::string& key, const std::string& value);

  int waitCompletion(const uint32_t timeout_ms=5000);
  int deleteKey(const std::string& key);
  int createKey(const std::string& key);

  int  applyConfiguration();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif