#include "MessagePSConsumer.h"
#ifdef KAFKA_RDK_ENABLE
#include "impl/kafka/rdk/MessagePSRDKafkaConsumer.h"
#endif

#ifdef KAFKA_ASIO_ENABLE
#include "impl/kafka/asio/MessagePSKafkaAsioConsumer.h"
#endif

#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSConsumer)
#define MRDDBG_ DBG_LOG(MessagePSConsumer)
#define MRDERR_ ERR_LOG(MessagePSConsumer)
#define MAXQUEUELEN 1000
namespace chaos {
namespace common {
namespace message {

/*   MessagePSConsumer::MessagePSConsumer(const std::string& clientid):MessagePublishSubscribeBase(clientid){
                
            };*/

MessagePSConsumer::MessagePSConsumer(const std::string& clientid, const std::string& gid, const std::string& k)
    : MessagePublishSubscribeBase(clientid), groupid(gid), defkey(k), msgs(MAXQUEUELEN),que_elem(0)
{

                                                                      };



static void removeElement(ele_t*p){
    delete p;
}
MessagePSConsumer::~MessagePSConsumer() {

     msgs.consume_all(removeElement);
              
}
int MessagePSConsumer::getMsgAsync(const std::string& key, const int32_t pnum) {
  return 0;
}

int MessagePSConsumer::getMsgAsync(const std::string& key, uint32_t off, const int32_t pnum) {
  return 0;
}
ele_uptr_t MessagePSConsumer::getMsg(int timeo) {
  ele_t* ele;

  if (msgs.empty()) {
    data_ready = false;
    MRDDBG_ << que_elem<<"] no messages yet, waiting..";

    if (waitCompletion(timeo) != 0) {
      MRDERR_ << " error waiting";
    }
  }
  if (msgs.pop(ele)) {
      que_elem--;
    if (ele) {
      return ele_uptr_t(ele);
    }
  }
  return ele_uptr_t();
}

int MessagePSConsumer::subscribe(const std::string& key) {
  std::string topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');

  keylist.insert(topic);
  MRDDBG_ <<keylist.size()<< "] subscribed to:"<<topic;

  return 0;
}

}  // namespace message
}  // namespace common
}  // namespace chaos