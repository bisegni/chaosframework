#include "MessagePSKafkaAsio.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaAsio)
#define MRDDBG_ DBG_LOG(MessagePSKafkaAsio)
#define MRDERR_ ERR_LOG(MessagePSKafkaAsio)

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


void MessagePSKafkaAsio::HandleMeta(const ::libkafka_asio::Connection::ErrorCodeType& err,const ::libkafka_asio::MetadataResponse::OptionalType& response){
   data_ready=true;
  cond.notify_all();

 if(err){
    MRDERR_
     << " META ["<<counter<<","<<oks<<","<<errs<<"]"<< boost::system::system_error(err).what();
 } else {
    MRDDBG_<< "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] META produced message!";
  stats.oks++;
 }
}

void MessagePSKafkaAsio::HandleRequest(const Connection::ErrorCodeType& err,
                   const ProduceResponse::OptionalType& response)
{
  data_ready=true;
  cond.notify_all();
   if(handlers[ONDELIVERY]){
      handlers[ONDELIVERY](msg_queue_t(),((err)?1:0));
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

MessagePSKafkaAsio::~MessagePSKafkaAsio() {
  running=false;
  th.join();
  if(connection!=NULL){
    delete connection;


  }
}

int MessagePSKafkaAsio::setOption(const std::string& key, const std::string& value) {
  return 0;
}

void MessagePSKafkaAsio::poll(){
  MRDDBG_<<"starting poll thread";
  running=true;
  int cnt=0;
  while(running){
  //  MRDDBG_<<"polling:"<<cnt++;

    ios.run();
  }
  MRDDBG_<<"exiting poll thread";

}

int MessagePSKafkaAsio::applyConfiguration() {
  char hostname[128];
  char errstr[512];

  if (servers.size() == 0) {
    MRDERR_ << " no server specified!";
    return -4;
  }
  int ret=0;

  if (gethostname(hostname, sizeof(hostname))) {
    MRDERR_ << "Failed to lookup hostname";
    return -1;
  }
  configuration.auto_connect = true;
  configuration.client_id = hostname;
  configuration.socket_timeout = 10000;
  //std::string first_host=*servers.begin();
  for(std::set<std::string>::iterator i=servers.begin();i!=servers.end();i++){
      MRDDBG_<<"broker:"<<*i;

    configuration.SetBrokerFromString(*i);
  }
  data_ready=false;
  //configuration.SetBrokerFromString(first_host);
  if(connection==NULL){
      MRDDBG_<<"connecting...";

    connection = new   Connection(ios, configuration);
    libkafka_asio::MetadataRequest request;
    connection->AsyncRequest(request,  boost::bind(&MessagePSKafkaAsio::HandleMeta,this,_1,_2));
    ios.run();
    ret=waitCompletion();
  }

 // th=boost::thread(&MessagePSKafkaAsio::poll,this);
  return ret;
}

  int MessagePSKafkaAsio::waitCompletion(const uint32_t timeo){
    boost::unique_lock<boost::mutex> guard(mutex_cond);
   //  boost::posix_time::milliseconds timeoutDuration( timeo ); //wait for 10 seconds
    MRDDBG_<<"wating operation";
    if(data_ready) return stats.last_err;
    if(!cond.timed_wait(guard,boost::posix_time::milliseconds(timeo))){
      MRDERR_<<"Timeout";
      return -100;
    }

    return stats.last_err; 
    
  }
int MessagePSKafkaAsio::createKey(const std::string& key){
  libkafka_asio::MetadataRequest request;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  
  data_ready=false;

  stats.last_err=0;

    MRDDBG_<<"create topic:"<<topic;

  request.AddTopicName(topic);
  connection->AsyncRequest(request,  boost::bind(&MessagePSKafkaAsio::HandleMeta,this,_1,_2));
  ios.run();
  return waitCompletion();
    
}

int MessagePSKafkaAsio::deleteKey(const std::string& key) {
  int err;
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  MRDDBG_<<"deleting key:"<<topic;

  request.ClearTopic(topic);
  data_ready=false;
  stats.last_err=0;
  connection->AsyncRequest(request,boost::bind(&MessagePSKafkaAsio::HandleRequest,this,_1,_2));
  ios.run();
  if(request.ResponseExpected()==false){
      cond.notify_all();
      data_ready=true;


  }
  return waitCompletion();

  
}

}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos