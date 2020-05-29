#include "MessagePSRDKafka.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSRDKafka)
#define MRDDBG_ DBG_LOG(MessagePSRDKafka)
#define MRDERR_ ERR_LOG(MessagePSRDKafka)

namespace chaos {
    namespace common {
        namespace message {
            MessagePSRDKafka::~MessagePSRDKafka(){

            }

            MessagePSRDKafka::MessagePSRDKafka(const std::string& clientid):MessagePublishSubscribeBase(clientid){
              conf = rd_kafka_conf_new();
              topic_conf = rd_kafka_topic_conf_new();


            }
            MessagePSRDKafka::MessagePSRDKafka(const std::string& clientid,const std::string& k):MessagePublishSubscribeBase(clientid),defkey(k){

            }

                
           
            int MessagePSRDKafka::setOption(const std::string&key,const std::string& value){
              return 0;
            }

            int MessagePSRDKafka::applyConfiguration(){
                char hostname[128];
                char errstr[512];


              if (gethostname(hostname, sizeof(hostname))) {
                MRDERR_<< "Failed to lookup hostname";
                return -1;
              }

              if (rd_kafka_conf_set(conf, "client.id", hostname,
                                    errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
                                                      MRDERR_<< "error:"<<errstr;

               return -2;
              }
              std::stringstream ss;
              for(std::set<std::string>::iterator i=servers.begin();i!=servers.end();i){
                ss<<*i;
                if((++i) != servers.end()){
                  ss<<",";
                }
              }


            if (rd_kafka_conf_set(conf, "bootstrap.servers", ss.str().c_str(),
                                  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
                                      MRDERR_<< "setting bootstrap servers:"<<errstr;
              return -4;
            }


            if (rd_kafka_topic_conf_set(topic_conf, "acks", "all",
                                  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
                                  MRDERR_<< "setting topic conf:"<<errstr;

                                  return -5;

            }
            return 0;
            }

            

        }}}