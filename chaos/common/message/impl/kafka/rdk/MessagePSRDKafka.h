/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PSRDKAFKA_H__
#define __MESSAGE_PSRDKAFKA_H__
#include <set>
#include <string>
#include <librdkafka/rdkafka.h>

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {
class MessagePSRDKafka {
 protected:
  rd_kafka_conf_t*       conf;
  rd_kafka_topic_conf_t* topic_conf;

 public:
  MessagePSRDKafka();
  ~MessagePSRDKafka();

  int setOption(const std::string& key, const std::string& value);

  int init(std::set<std::string>& servers);
  int 
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif