#include "MessagePSKafkaAsioProducer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MMessagePSKafkaAsioProducer)
#define MRDDBG_ DBG_LOG(MMessagePSKafkaAsioProducer)
#define MRDERR_ ERR_LOG(MMessagePSKafkaAsioProducer)

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

MMessagePSKafkaAsioProducer::~MMessagePSKafkaAsioProducer() {
  if(connection){
    delete connection;
  }
}

MMessagePSKafkaAsioProducer::MMessagePSKafkaAsioProducer():connection(NULL) {

  running = false;
}
MMessagePSKafkaAsioProducer::MMessagePSKafkaAsioProducer(const std::string& k):connection(NULL) {
}
int MMessagePSKafkaAsioProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  return 0;
}

void MMessagePSKafkaAsioProducer::addServer(const std::string& url) {
  MessagePublishSubscribeBase::addServer(url);
}

int MMessagePSKafkaAsioProducer::applyConfiguration() {
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
  return ret;
}

int MMessagePSKafkaAsioProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  int err;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  counter++;
  std::replace(topic.begin(), topic.end(), '/', '.');
  ProduceRequest request;
  
  request.AddValue(std::string(data.getBSONRawData(),size), topic, 0);
  connection->AsyncRequest(request, &HandleRequest);

  return err;
}
void MMessagePSKafkaAsioProducer::poll() {
}
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos