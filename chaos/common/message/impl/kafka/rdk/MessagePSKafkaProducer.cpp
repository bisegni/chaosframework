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
  if (opaque) {
  }
  if (rkmessage->err)
    //fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
    MRDERR_ << "Message delivery failed:" << rd_kafka_err2str(rkmessage->err);
  /* The rkmessage is destroyed automatically by librdkafka */
}
MessagePSKafkaProducer::~MessagePSKafkaProducer() {
  fprintf(stderr, "%% Flushing final messages..\n");
  rd_kafka_flush(rk, 10 * 1000 /* wait for max 10 seconds */);

  /* If the output queue is still not empty there is an issue
         * with producing messages to the clusters. */
  if (rd_kafka_outq_len(rk) > 0)
    fprintf(stderr, "%% %d message(s) were not delivered\n", rd_kafka_outq_len(rk));

  /* Destroy the producer instance */
  rd_kafka_destroy(rk);
}

MessagePSKafkaProducer::MessagePSKafkaProducer() {
  running = false;
}
MessagePSKafkaProducer::MessagePSKafkaProducer(const std::string& k) {
}
int MessagePSKafkaProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  return 0;
}

void MessagePSKafkaProducer::addServer(const std::string& url) {
  MessagePublishSubscribeBase::addServer(url);
}

int MessagePSKafkaProducer::applyConfiguration() {
  char errstr[512];
  int  ret = 0;
  if ((ret = MessagePSRDKafka::init(servers)) == 0) {
    if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr)))) {
      MRDERR_ << "Failed to create new producer: " << errstr;
      return -10;
    }
    if (handlers.size()) {
      rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    }
  }
  counter = 0;
  return ret;
}

int MessagePSKafkaProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  rd_kafka_resp_err_t err;
  int                 size  = data.getBSONRawSize();
  std::string         topic = key;
  counter++;
  std::replace(topic.begin(), topic.end(), '/', '.');

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

    MRDERR_ << counter << "] Failed to produce to topic " << topic << rd_kafka_err2str(err);

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

  return err;
}
void MessagePSKafkaProducer::poll() {
  rd_kafka_poll(rk, 0 /*non-blocking*/);
}
}  // namespace rdk
}  // namespace kafka
}  // namespace message
}  // namespace common
}  // namespace chaos