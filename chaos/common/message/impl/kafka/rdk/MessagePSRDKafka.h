/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PSRDKAFKA_H__
#define __MESSAGE_PSRDKAFKA_H__
#include <set>
#include <string>
#include <librdkafka/rdkafka.h>
#include <chaos/common/message/MessagePublishSubscribeBase.h>

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {
class MessagePSRDKafka:public virtual MessagePublishSubscribeBase {
 protected:
  rd_kafka_conf_t*       conf;
  rd_kafka_topic_conf_t* topic_conf;
  rd_kafka_t* rk;

  public:
  MessagePSRDKafka();
  ~MessagePSRDKafka();

  int setOption(const std::string& key, const std::string& value);

  int init(std::set<std::string>& servers);

  
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

  int setMaxMsgSize(const int size);
  void start();
  void stop();
  virtual void poll();
};
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos
#endif