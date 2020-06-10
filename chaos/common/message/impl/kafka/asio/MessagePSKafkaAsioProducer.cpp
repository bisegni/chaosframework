#include "MessagePSKafkaAsioProducer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaAsioProducer)
#define MRDDBG_ DBG_LOG(MessagePSKafkaAsioProducer)
#define MRDERR_ ERR_LOG(MessagePSKafkaAsioProducer)

using libkafka_asio::Connection;
using libkafka_asio::ProduceRequest;
using libkafka_asio::ProduceResponse;


namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {


void MessagePSKafkaAsioProducer::HandleRequest(const Connection::ErrorCodeType& err,
                   const ProduceResponse::OptionalType& response)
{
  cond.notify_all();
   if(handlers[ONDELIVERY]){
      handlers[ONDELIVERY](std::vector<chaos::common::data::CDWUniquePtr>(),((err)?1:0));
  }
  if (err)
  {
    MRDERR_
     << "["<<counter<<","<<oks<<","<<errs<<"]"<< boost::system::system_error(err).what();
    stats.errs++;
   
    return;
  }
  //MRDDBG_<< "["<<counter<<","<<sentOk<<","<<sentErr<<"] Successfully produced message!";
  stats.oks++;
}

MessagePSKafkaAsioProducer::~MessagePSKafkaAsioProducer() {
}

MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer() {
}
MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer(const std::string& k){
}
int MessagePSKafkaAsioProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum) {
  return 0;
}

int MessagePSKafkaAsioProducer::applyConfiguration() {
  return MessagePSKafkaAsio::init(servers);
}


int MessagePSKafkaAsioProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum) {
  int err;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  
  request.AddValue(std::string(data.getBSONRawData(),size), topic, pnum);
  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioProducer::HandleRequest,this,_1,_2));

  return err;
}

int MessagePSKafkaAsioProducer::deleteKey(const std::string& key) {
  int err;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  
  request.ClearTopic(topic);
  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioProducer::HandleRequest,this,_1,_2));
  
  return err;
}

}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos