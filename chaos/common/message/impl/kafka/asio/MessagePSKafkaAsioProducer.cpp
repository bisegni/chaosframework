#include "MessagePSKafkaAsioProducer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaAsioProducer)
#define MRDDBG_ DBG_LOG(MessagePSKafkaAsioProducer)
#define MRDERR_ ERR_LOG(MessagePSKafkaAsioProducer)

using libkafka_asio::Connection;
using libkafka_asio::ProduceRequest;
using libkafka_asio::ProduceResponse;
using libkafka_asio::MetadataResponse;
using libkafka_asio::MetadataResponse;


namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

static void cb(const Connection::ErrorCodeType& err,
                   const ProduceResponse::OptionalType& response)
{
  if (err)
  {
    std::cerr
      << "Error: " << boost::system::system_error(err).what()
      << std::endl;
    return;
  }
  std::cout << "Successfully!!" << std::endl;
}
void MessagePSKafkaAsioProducer::HandleMeta(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::MetadataResponse::OptionalType& response){
 if(err){
    MRDERR_
     << " META ["<<counter<<","<<oks<<","<<errs<<"]"<< boost::system::system_error(err).what();
 } else {
    MRDDBG_<< "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] META produced message!";
  stats.oks++;
 }
}

void MessagePSKafkaAsioProducer::HandleRequest(const Connection::ErrorCodeType& err,
                   const ProduceResponse::OptionalType& response)
{
  data_ready=true;
  cond.notify_all();
   if(handlers[ONDELIVERY]){
      handlers[ONDELIVERY](std::vector<chaos::common::data::CDWUniquePtr>(),((err)?1:0));
  }
  if (err)
  {
    MRDERR_
     << "["<<counter<<","<<oks<<","<<errs<<"]"<< boost::system::system_error(err).what();
    stats.errs++;
    stats.last_err++;
    return;
  }
  MRDDBG_<< "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] Successfully produced message!";
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
  int err=0;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
 /* if(stats.counter==0){
    libkafka_asio::MetadataRequest request;
    request.AddTopicName(topic);

    connection->AsyncRequest(request,  boost::bind(&MessagePSKafkaAsioProducer::HandleMeta,this,_1,_2));
     ios.run();
  }*/
  ProduceRequest request;
  
  request.AddValue(std::string(data.getBSONRawData(),size), topic, pnum);
  MRDDBG_<< "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] Send size:"<<size<<" topic:"<<topic<<" p:"<<pnum;
  data_ready=false;

  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioProducer::HandleRequest,this,_1,_2));
  ios.run();
  
  err=stats.last_err;
  if(stats.last_err){
       stats.last_err=0;
  }
  return err;
}

int MessagePSKafkaAsioProducer::deleteKey(const std::string& key) {
  int err;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  
  request.ClearTopic(topic);
  data_ready=false;

  connection->AsyncRequest(request,boost::bind(&MessagePSKafkaAsioProducer::HandleRequest,this,_1,_2));
  ios.run();
  if(request.ResponseExpected()==false){
      cond.notify_all();

  }

  err=stats.last_err;
  if(stats.last_err){
       stats.last_err=0;
  }
  return err;
}

}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos