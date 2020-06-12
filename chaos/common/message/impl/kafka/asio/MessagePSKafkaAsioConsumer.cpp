#include "MessagePSKafkaAsioConsumer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaAsioConsumer)
#define MRDDBG_ DBG_LOG(MessagePSKafkaAsioConsumer)
#define MRDERR_ ERR_LOG(MessagePSKafkaAsioConsumer)
using libkafka_asio::Connection;

using libkafka_asio::FetchRequest;
using libkafka_asio::FetchResponse;
using libkafka_asio::MessageAndOffset;

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {


void MessagePSKafkaAsioConsumer::HandleRequest(const Connection::ErrorCodeType& err,
                   const FetchResponse::OptionalType& response)
{

    if(err==0){
        int before=stats.counter;

        for(FetchResponse::const_iterator i=response->begin();i!=response->end();i++){
          stats.counter++;
          try{
          const MessageAndOffset& msg=*i;
          const libkafka_asio::Bytes& bytes=msg.value();
          
          chaos::common::data::CDWShrdPtr t(new chaos::common::data::CDataWrapper((const char*) &(*bytes)[0], bytes->size()));
          msgs.push_back(t);
          stats.oks++;
          } catch(...){
                MRDERR_<<" Not valid bson data";
            stats.last_err=1;

          }
        }
         MRDDBG_<<"Retrieved:"<<(stats.counter-before);

    }
    data_ready=true;
  cond.notify_all();

    
    if(handlers[ONARRIVE]){
       
      handlers[ONARRIVE](msgs,((err)?1:0));
    }

  if (err)
  {
    MRDERR_
     << "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"]"<< boost::system::system_error(err).what();
    stats.errs++;
      stats.last_err=1;

    return;
  }

  //MRDDBG_<< "["<<counter<<","<<sentOk<<","<<sentErr<<"] Successfully produced message!";
}

MessagePSKafkaAsioConsumer::~MessagePSKafkaAsioConsumer() {
}

MessagePSKafkaAsioConsumer::MessagePSKafkaAsioConsumer():chaos::common::message::MessagePublishSubscribeBase("asio") {
}
MessagePSKafkaAsioConsumer::MessagePSKafkaAsioConsumer(const std::string& k):chaos::common::message::MessagePublishSubscribeBase("asio") {
}
int MessagePSKafkaAsioConsumer::getMsgAsync(const std::string&key,const int32_t pnum){
  FetchRequest request;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
 
  request.FetchTopic(topic, pnum);
  stats.last_err=0;
  // Send the prepared fetch request.
  // The connection will attempt to automatically connect to the broker,
  // specified in the configuration.
  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioConsumer::HandleRequest,this,_1,_2));
  ios.reset();
  ios.run();
  return stats.last_err;
}

int MessagePSKafkaAsioConsumer::getMsgAsync(const std::string&key,uint32_t off,const int32_t pnum){
  FetchRequest request;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
 
  request.FetchTopic(topic, pnum,off);
  stats.last_err=0;

  // Send the prepared fetch request.
  // The connection will attempt to automatically connect to the broker,
  // specified in the configuration.
  connection->AsyncRequest(request, boost::bind(&MessagePSKafkaAsioConsumer::HandleRequest,this,_1,_2));
  ios.reset();
  ios.run();
  return stats.last_err;
}


}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos