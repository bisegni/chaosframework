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


void HandleRequest(const Connection::ErrorCodeType& err,
                   const ProduceResponse::OptionalType& response)
{
  if (err)
  {
    MRDERR_
      << "Error: " << boost::system::system_error(err).what()
      << std::endl;
    return;
  }
  MRDDBG_<< "Successfully produced message!" << std::endl;
}

MessagePSKafkaAsioProducer::~MessagePSKafkaAsioProducer() {
  running=false;
  ios.stop();
  th.join();
  if(connection){
    delete connection;
  }
}

MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer():connection(NULL) {

  running = false;
}
MessagePSKafkaAsioProducer::MessagePSKafkaAsioProducer(const std::string& k):connection(NULL) {
}
int MessagePSKafkaAsioProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  return 0;
}

void MessagePSKafkaAsioProducer::addServer(const std::string& url) {
  MessagePublishSubscribeBase::addServer(url);
}

void MessagePSKafkaAsioProducer::poll(){
  MRDDBG_<<"starting poll thread";
  running=true;
  while(running){
    MRDDBG_<<"entering run";

    ios.run();
    MRDDBG_<<"exiting run";

  }
    MRDDBG_<<"exiting poll thread";

}
int MessagePSKafkaAsioProducer::applyConfiguration() {
  int ret=0;
  char hostname[128];
  char errstr[512];

  if (servers.size() == 0) {
    MRDERR_ << " no server specified!";
    return -4;
  }
  if (gethostname(hostname, sizeof(hostname))) {
    MRDERR_ << "Failed to lookup hostname";
    return -1;
  }
  configuration.auto_connect = true;
  configuration.client_id = hostname;
  configuration.socket_timeout = 10000;
  configuration.SetBrokerFromString(*servers.begin());
  if(connection==NULL){
    connection = new   Connection(ios, configuration);

  }

  counter = 0;
 // th=boost::thread(&MessagePSKafkaAsioProducer::poll,this);
  return ret;
}

int MessagePSKafkaAsioProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  int err;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  counter++;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  
  request.AddValue(std::string(data.getBSONRawData(),size), topic, 0);
  connection->AsyncRequest(request, &HandleRequest);
  ios.run();

  return err;
}

}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos