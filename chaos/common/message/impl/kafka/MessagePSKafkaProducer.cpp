#include <librdkafka/rdkafka.h>
#include "MessagePSKafkaProducer.h"
#include <chaos/common/global.h>
#define MRDAPP_ INFO_LOG(MessagePSKafkaProducer)
#define MRDDBG_ DBG_LOG(MessagePSKafkaProducer)
#define MRDERR_ ERR_LOG(MessagePSKafkaProducer)

namespace chaos {
namespace common {
namespace message {

static void dr_msg_cb(rd_kafka_t*               rk,
                      const rd_kafka_message_t* rkmessage,
                      void*                     opaque) {
  if (opaque) {
  }
  if (rkmessage->err)
    fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));

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

MessagePSKafkaProducer::MessagePSKafkaProducer(const std::string& clientid)
    : MessagePSRDKafka(clientid) {
  running = false;
}
MessagePSKafkaProducer::MessagePSKafkaProducer(const std::string& clientid, const std::string& k)
    : MessagePSRDKafka(clientid), defkey(k) {
}
int MessagePSKafkaProducer::pushMsg(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  return 0;
}


int MessagePSKafkaProducer::applyConfiguration() {
  char errstr[512];

  if (MessagePSRDKafka::applyConfiguration() == 0) {
    if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr)))) {
      MRDERR_ << "Failed to create new producer: " << errstr;
      return -10;
    }
    if (handlers.size()) {
      rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    }
  }
  return 0;
}

int MessagePSKafkaProducer::pushMsgAsync(const chaos::common::data::CDataWrapper& data, const std::string& key) {
  int err;

  err = rd_kafka_producev(
      /* Producer handle */
      rk,
      /* Topic name */
      RD_KAFKA_V_TOPIC(key.c_str()),
      /* Make a copy of the payload. */
      RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
      /* Message value and length */
      RD_KAFKA_V_VALUE((void*)data.getBSONRawData(), data.getBSONRawSize()),
      /* Per-Message opaque, provided in
                         * delivery report callback as
                         * msg_opaque. */
      RD_KAFKA_V_OPAQUE(this),
      /* End sentinel */
      RD_KAFKA_V_END);
      rd_kafka_poll(rk, 0/*non-blocking*/);

  return err;
}
void MessagePSKafkaProducer::poll() {
        rd_kafka_poll(rk, 0/*non-blocking*/);

}
}  // namespace message
}  // namespace common
}  // namespace chaos