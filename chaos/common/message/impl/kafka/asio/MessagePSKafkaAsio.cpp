#include "MessagePSKafkaAsio.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaAsio)
#define MRDDBG_ DBG_LOG(MessagePSKafkaAsio)
#define MRDERR_ ERR_LOG(MessagePSKafkaAsio)

using libkafka_asio::Connection;
using libkafka_asio::ProduceRequest;
using libkafka_asio::ProduceResponse;

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace asio {

MessagePSKafkaAsio::~MessagePSKafkaAsio() {
  running=false;
  th.join();
  if(connection!=NULL){
    delete connection;


  }
}

MessagePSKafkaAsio::MessagePSKafkaAsio():connection(NULL),running(false),data_ready(false) {
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

int MessagePSKafkaAsio::init(std::set<std::string>& servers) {
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
  configuration.SetBrokerFromString(*servers.begin());
  if(connection==NULL){
    connection = new   Connection(ios, configuration);

  }

 // th=boost::thread(&MessagePSKafkaAsio::poll,this);
  return ret;
}

  int MessagePSKafkaAsio::waitCompletion(const uint32_t timeo){
    boost::unique_lock<boost::mutex> guard(mutex_cond);
   //  boost::posix_time::milliseconds timeoutDuration( timeo ); //wait for 10 seconds
    MRDDBG_<<"wating operation";
    if(data_ready) return 0;
    if(!cond.timed_wait(guard,boost::posix_time::milliseconds(timeo))){
      MRDERR_<<"Timeout";
      return -100;
    }
    return 0;
  }


}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos