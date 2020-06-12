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


MessagePSKafkaAsioProducer::~MessagePSKafkaAsioProducer() {
}

MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer():chaos::common::message::MessagePublishSubscribeBase("asio") {
}
MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer(const std::string& k):chaos::common::message::MessagePublishSubscribeBase("asio"){
}
int MessagePSKafkaAsioProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum) {
  return 0;
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
//    boost::asio::io_service ioss;

  request.AddValue(std::string(data.getBSONRawData(),size), topic, pnum);
  MRDDBG_<< "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] Send size:"<<size<<" topic:"<<topic<<" p:"<<pnum;
  data_ready=false;
  //Connection conn(ioss, configuration);
  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioProducer::HandleRequest,this,_1,_2));
  //if(ios.stopped()){
     // MRDDBG_<<"resetting ios";

    ios.reset();
 // }
  ios.run();
  
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