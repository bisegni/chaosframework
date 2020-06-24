/*
 * Copyright 2020 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__QueryDataMsgPSConsumer__
#define __CHAOSFramework__QueryDataMsgPSConsumer__

#include <chaos/common/message/MessagePSConsumer.h>
#include <chaos/common/message/MessagePSDriver.h>
#include "QueryDataConsumer.h"

namespace chaos {
namespace metadata_service {

class QueryDataMsgPSConsumer : public QueryDataConsumer {
  std::string                             msgbrokerdrv;
  std::string                             msgbroker;
  std::string                             groupid;
  std::map<std::string, uint64_t>         alive_map;
  chaos::common::message::consumer_uptr_t cons;
  void                                    messageHandler(const chaos::common::message::ele_t& data);
  //---------------- DirectIODeviceServerChannelHandler -----------------------
  
 public:
  QueryDataMsgPSConsumer(const std::string& id):groupid(id) {
    if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_HA_ZONE_NAME)) {
    groupid = groupid + "_" + GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_HA_ZONE_NAME);
  }

    msgbrokerdrv = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_MSG_BROKER_DRIVER);

    cons = chaos::common::message::MessagePSDriver::getConsumerDriver(msgbrokerdrv, groupid);
  }
  ~QueryDataMsgPSConsumer(){}
  void init(void* init_data);
  void start();
  void stop();
  void deinit();
 /* int consumePutEvent(const std::string&              key,
                      const uint8_t                   hst_tag,
                      const ChaosStringSetConstSPtr   meta_tag_set,
                      chaos::common::data::BufferSPtr channel_data);
*/
  int consumeHealthDataEvent(const std::string&              key,
                             const uint8_t                   hst_tag,
                             const ChaosStringSetConstSPtr   meta_tag_set,
                             chaos::common::data::BufferSPtr channel_data);
  /*int consumePutEvent(const std::string& key,
                                const uint8_t hst_tag,
                                const ChaosStringSetConstSPtr meta_tag_set,
                                chaos::common::data::CDataWrapper& channel_data);
           
    */        
  int consumeGetEvent(chaos::common::data::BufferSPtr key_data,
                                uint32_t key_len,
                                opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult& result_header,
                                chaos::common::data::BufferSPtr& result_value);
            
  int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode& header,
                                const ChaosStringVector& keys,
                                opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult& result_header,
                                chaos::common::data::BufferSPtr& result_value,
                                uint32_t& result_value_len);
            
  int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                      const std::string& search_key,
                                      const ChaosStringSet& meta_tags,
                                      const ChaosStringSet& projection_keys,
                                      const uint64_t search_start_ts,
                                      const uint64_t search_end_ts,
                                      opcode_headers::SearchSequence& last_element_found_seq,
                                      opcode_headers::QueryResultPage& page_element_found);
            
  int consumeDataIndexCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                           const std::string& search_key,
                                           const ChaosStringSet& meta_tags,
                                           const ChaosStringSet& projection_keys,
                                           const uint64_t search_start_ts,
                                           const uint64_t search_end_ts,
                                           opcode_headers::SearchSequence& last_element_found_seq,
                                           opcode_headers::QueryResultPage& page_element_found);
            
            
  int consumeDataCloudDelete(const std::string& search_key,
                                       uint64_t start_ts,
                                       uint64_t end_ts);

  int countDataCloud(const std::string& search_key,
                                       uint64_t start_ts,
                                       uint64_t end_ts,
                                       uint64_t& count);
            
            
            //---------------- DirectIOSystemAPIServerChannelHandler -----------------------
    int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader& header,
                                               const std::string& producer_id,
                                               chaos::common::data::BufferSPtr& channel_found_data,
                                               DirectIOSystemAPISnapshotResultHeader &result_header);
            
    int consumeLogEntries(const std::string& node_name,
                                  const ChaosStringVector& log_entries);

};
}  // namespace metadata_service
}  // namespace chaos

#endif /* defined(__CHAOSFramework__QueryDataMsgPSConsumer__) */
