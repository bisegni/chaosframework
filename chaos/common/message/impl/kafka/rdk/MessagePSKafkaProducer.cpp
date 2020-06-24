#include "MessagePSKafkaProducer.h"
#include <chaos/common/global.h>
#include <librdkafka/rdkafka.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaProducer)
#define MRDDBG_ DBG_LOG(MessagePSKafkaProducer)
#define MRDERR_ ERR_LOG(MessagePSKafkaProducer)

namespace chaos {
namespace common {
namespace message {
namespace kafka {
namespace rdk {

static void dr_msg_cb(rd_kafka_t*               rk,
                      const rd_kafka_message_t* rkmessage,
                      void*                     opaque) {
    
     MessagePSKafkaProducer* mp=(MessagePSKafkaProducer*)opaque;
     mp->HandleRequest(rk,rkmessage);                   
}
void MessagePSKafkaProducer::HandleRequest(rd_kafka_t*               rk,
                      const rd_kafka_message_t* rkmessage){
/**/
  
  if (rkmessage->err){

    stats.last_err=1;
    stats.errs++;
    MRDERR_ << "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"]" << " Message delivery failed:" << rd_kafka_err2str(rkmessage->err);
    MRDERR_<<" key:"<<rkmessage->key<<" len:"<<rkmessage->len<<" off:"<<rkmessage->offset<< " part:"<<rkmessage->partition;

    return;
  }
  stats.counter++;
  stats.last_err=0;
  stats.oks++;


    //fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
  /* The rkmessage is destroyed automatically by librdkafka */
}
MessagePSKafkaProducer::~MessagePSKafkaProducer() {
  flush();
  /* If the output queue is still not empty there is an issue
         * with producing messages to the clusters. */
  
  /* Destroy the producer instance */
  rd_kafka_destroy(rk);
}
int MessagePSKafkaProducer::flush(const int timeo){
  MRDDBG_ << "Flushing... ";

    rd_kafka_flush(rk, timeo);
if (rd_kafka_outq_len(rk) > 0){
    fprintf(stderr, "%% %d message(s) were not delivered\n", rd_kafka_outq_len(rk));

    return -1;
}
return 0;

}

MessagePSKafkaProducer::MessagePSKafkaProducer():chaos::common::message::MessagePublishSubscribeBase("kafka-rdk") {
  running = false;
}
MessagePSKafkaProducer::MessagePSKafkaProducer(const std::string& k):chaos::common::message::MessagePublishSubscribeBase("kafka-rdk") {
}
int MessagePSKafkaProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum) {
  return 0;
}
  int MessagePSKafkaProducer::deleteKey(const std::string&key){
    return 0;
  }


int MessagePSKafkaProducer::applyConfiguration() {
  int  ret = 0;
  char ers[512];
  if ((ret = MessagePSRDKafka::init(servers)) == 0) {
    setMaxMsgSize(1024*1024);

    if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, ers, sizeof(ers)))) {
      errstr=ers;
      MRDERR_ << "Failed to create new producer: " << errstr;
      return -10;
    }
    if (handlers.size()) {
      rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    }
  }
  return ret;
}


int MessagePSKafkaProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key,const int32_t pnum) {
  rd_kafka_resp_err_t err;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  std::replace(topic.begin(), topic.end(), '/', '.');
  if(rk==NULL){
      MRDERR_ << "Not applied configuration" << errstr;
      errstr="Not applied configuration";
      return -11;
  }
//MRDDBG_ << "pushing " << size;
retry:

  err = rd_kafka_producev(
      /* Producer handle */
      rk,
      /* Topic name */
      RD_KAFKA_V_TOPIC(topic.c_str()),
      /* Make a copy of the payload. */
      RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
      /* Message value and length */
      RD_KAFKA_V_VALUE((void*)data.getBSONRawData(), size),
      /* Per-Message opaque, provided in
                         * delivery report callback as
                         * msg_opaque. */
      RD_KAFKA_V_OPAQUE(this),
      /* End sentinel */
      RD_KAFKA_V_END);

  if (err) {
    /*
                         * Failed to *enqueue* message for producing.
                         */

    MRDERR_ << "["<<stats.counter<<","<<stats.oks<<","<<stats.errs<<"] Failed to produce to topic " << topic << rd_kafka_err2str(err)<<" size:"<<size;

    if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
      /* If the internal queue is full, wait for
                                 * messages to be delivered and then retry.
                                 * The internal queue represents both
                                 * messages to be sent and messages that have
                                 * been sent or failed, awaiting their
                                 * delivery report callback to be called.
                                 *
                                 * The internal queue is limited by the
                                 * configuration property
                                 * queue.buffering.max.messages */
      rd_kafka_poll(rk, 100 /*block for max 1000ms*/);

      goto retry;
    }
  }
  rd_kafka_poll(rk, 0 /*non-blocking*/);
  stats.counter++;

  if(err==0){
    stats.oks++;
    return stats.last_err;
  } else {
    stats.errs++;

  }
  return err;
}
void MessagePSKafkaProducer::poll() {
      rd_kafka_poll(rk, 500 /*block for max 1000ms*/);
}
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos