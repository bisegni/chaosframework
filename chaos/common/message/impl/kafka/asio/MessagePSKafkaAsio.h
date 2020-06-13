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
  uint32_t last_offset;
  uint32_t first_offset;
  uint32_t current_offset;
inline void prepareRequest(const std::string&key,const std::string& broker="",uint32_t off=0,uint32_t par=0){
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  data_ready=false;
  stats.key=topic;
  stats.offset=off;
  stats.partition=par;
  stats.last_err=0;
   
}

void HandleOffset(const ::libkafka_asio::Connection::ErrorCodeType& err,
                   const ::libkafka_asio::OffsetResponse::OptionalType& response);

 void HandleMeta(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::MetadataResponse::OptionalType& response);
void HandleRequest(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::ProduceResponse::OptionalType& response);

 
void poll();

 public:
  MessagePSKafkaAsio():MessagePublishSubscribeBase("asio"),connection(NULL),running(false),data_ready(false),first_offset(0),last_offset(0),current_offset(0) {
}

  ~MessagePSKafkaAsio();

  int setOption(const std::string& key, const std::string& value);

  int waitCompletion(const uint32_t timeout_ms=5000);
  int deleteKey(const std::string& key);
  int createKey(const std::string& key);
  
  /**
   * @brief get the offset of the specified topic partition
   * @param off returned offset
   * @param type (-1 earliest, -2 latest)
   * @param par partition
   * @return 0 on success
   */
  int getOffset(const std::string& key,uint32_t &off,int type=-2,int par=0);

  int  applyConfiguration();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif