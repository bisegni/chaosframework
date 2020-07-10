#include "MessagePSRDKafkaConsumer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSRDKafkaConsumer)
#define MRDDBG_ DBG_LOG(MessagePSRDKafkaConsumer)
#define MRDERR_ ERR_LOG(MessagePSRDKafkaConsumer)

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {

/*
void MessagePSRDKafkaConsumer::HandleRequest(const Connection::ErrorCodeType& err,
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
*/

MessagePSRDKafkaConsumer::~MessagePSRDKafkaConsumer() {
  MRDDBG_<<" DESTROY CONSUMER";
  if(rk){
    rd_kafka_consumer_close(rk);


        /* Destroy the consumer */
    rd_kafka_destroy(rk);
  }
}


MessagePSRDKafkaConsumer::MessagePSRDKafkaConsumer(const std::string& gid,const std::string& defkey): chaos::common::message::MessagePSConsumer("kafka-rdk",gid,defkey),chaos::common::message::MessagePublishSubscribeBase("kafka-rdk") {
}
int MessagePSRDKafkaConsumer::getMsgAsync(const std::string&key,const int32_t pnum){
  
  return stats.last_err;
}

int MessagePSRDKafkaConsumer::getMsgAsync(const std::string&key,uint32_t off,const int32_t pnum){
  
  return stats.last_err;
}
int MessagePSRDKafkaConsumer::applyConfiguration() {
  char ers[512];
  int  ret = 0;
   if ((ret = MessagePSRDKafka::init(servers)) == 0) {
    

        /* If there is no previously committed offset for a partition
         * the auto.offset.reset strategy will be used to decide where
         * in the partition to start fetching messages.
         * By setting this to earliest the consumer will read all messages
         * in the partition if there was no previously committed offset. */
      /*  if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest",
                              errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
                MRDERR_<<errstr;
                rd_kafka_conf_destroy(conf);
        return -2;
    }*/
    if(rk==NULL){
      if(groupid!=""){
      if (rd_kafka_conf_set(conf, "group.id", groupid.c_str(),
                                ers, sizeof(ers)) != RD_KAFKA_CONF_OK) {

                  MRDERR_<<ers;
                  errstr=ers;
                // rd_kafka_conf_destroy(conf);
                  return -1;
      }
    }
      if (!(rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, ers, sizeof(ers)))) {
        MRDERR_ << "Failed to create new consumer: " << ers;
        errstr=ers;

        return -10;
      }
    }

        /* Redirect all messages from per-partition queues to
         * the main queue so that messages can be consumed with one
         * call from all assigned partitions.
         *
         * The alternative is to poll the main queue (for events)
         * and each partition queue separately, which requires setting
         * up a rebalance callback and keeping track of the assignment:
         * but that is more complex and typically not recommended. */
       rd_kafka_poll_set_consumer(rk);
      
   
  }
  return ret;
}
int MessagePSRDKafkaConsumer::subscribe(const std::string& key){
  MessagePSConsumer::subscribe(key);
  if(rk==NULL){
    errstr="apply configuration first!";
     MRDERR_<<errstr;
    return -5;
  }
  rd_kafka_topic_partition_list_t *subscription; 
  subscription = rd_kafka_topic_partition_list_new(keylist.size());
  for(std::set<std::string>::iterator i=keylist.begin();i!=keylist.end();i++){
        rd_kafka_topic_partition_list_add(subscription,
                                                  (*i).c_str(),
                                                  /* the partition is ignored
                                                   * by subscribe() */
                                                  RD_KAFKA_PARTITION_UA);
        MRDDBG_<<" subscribing to "<<*i;
                                          
      }
       rd_kafka_resp_err_t err = rd_kafka_subscribe(rk, subscription);
        if (err) {
          MRDERR_<<"Failed to subscribe to "<<subscription->cnt<<" topics, group:\""<<groupid<<"\" err:"<<rd_kafka_err2str(err);
          errstr=rd_kafka_err2str(err);
          rd_kafka_topic_partition_list_destroy(subscription);
          rd_kafka_destroy(rk);
          return -20;
        }

        MRDDBG_<<" subscribed to "<<subscription->cnt;
      

        rd_kafka_topic_partition_list_destroy(subscription);
        return 0;

}

void MessagePSRDKafkaConsumer::poll(){
  rd_kafka_message_t *rkm;
  if(rk==NULL){
      MRDERR_ << "Not applied configuration" << errstr;
      errstr="Not applied configuration";
      sleep(1);
      return;
  }
  rkm = rd_kafka_consumer_poll(rk, 100);
      if (!rkm)
              return; /* Timeout: no message within 100ms,
                          *  try again. This short timeout allows
                          *  checking for `run` at frequent intervals.
                          */

      /* consumer_poll() will return either a proper message
        * or a consumer error (rkm->err is set). */

      if (rkm->err) {
              /* Consumer errors are generally to be considered
                * informational as the consumer will automatically
                * try to recover from all types of errors. */
        errstr=rd_kafka_message_errstr(rkm);
        MRDERR_<<"Consumer error:"<<errstr;
        stats.errs++;
        rd_kafka_message_destroy(rkm);
        return;

      }
      stats.counter++;

    //  MRDDBG_<<" message from:"<<rd_kafka_topic_name(rkm->rkt)<<" par:"<<rkm->partition<<" off:"<<rkm->offset;
     
      /* Print the message key. */
    /*  if (rkm->key && is_printable(rkm->key, rkm->key_len))
              printf(" Key: %.*s\n",
                      (int)rkm->key_len, (const char *)rkm->key);
      else if (rkm->key)
              printf(" Key: (%d bytes)\n", (int)rkm->key_len);
      */
        if(rkm->payload && rkm->len){
        try{
        //  msgs.push_back(t);
          if(handlers[ONARRIVE]){
            ele_t d;
            d.key=rd_kafka_topic_name(rkm->rkt);
            d.off=rkm->offset;
            d.par=rkm->partition;
            d.cd= chaos::common::data::CDWShrdPtr( new chaos::common::data::CDataWrapper((const char*) rkm->payload, rkm->len));

            handlers[ONARRIVE](d);
          } else {
            ele_t *ele =new ele_t();
            ele->key=rd_kafka_topic_name(rkm->rkt);
            ele->off=rkm->offset;
            ele->par=rkm->partition;
            ele->cd= chaos::common::data::CDWShrdPtr( new chaos::common::data::CDataWrapper((const char*) rkm->payload, rkm->len));
            msgs.push(ele);
            que_elem++;
            stats.oks++;
            data_ready=true;
            cond.notify_all();
          }
          stats.oks++;
        } catch(...){
          stats.errs++;
          MRDERR_<<" invalid chaos packet from:"<<rd_kafka_topic_name(rkm->rkt)<< " len:"<<rkm->len<<" string:"<<std::string((const char*)rkm->payload, rkm->len);

        }
        } 
      /* Print the message value/payload. */
     /* if (rkm->payload && is_printable(rkm->payload, rkm->len))
              printf(" Value: %.*s\n",
                      (int)rkm->len, (const char *)rkm->payload);
      else if (rkm->key)
              printf(" Value: (%d bytes)\n", (int)rkm->len);
      */
      rd_kafka_message_destroy(rkm);

}


}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos