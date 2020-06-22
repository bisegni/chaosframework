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

#include "QueryDataConsumer.h"
#include <chaos/common/message/MessagePSConsumer.h>
#include <chaos/common/message/MessagePSDriver.h>


namespace chaos{
    namespace metadata_service {
        
        class QueryDataMsgPSConsumer: public QueryDataConsumer {
            std::string msgbrokerdrv;
            std::string msgbroker;
            std::string groupid;
            std::map<std::string,uint64_t> alive_map;
            chaos::common::message::consumer_uptr_t cons;
            void messageHandler(const chaos::common::message::ele_t&data);
            //---------------- DirectIODeviceServerChannelHandler -----------------------
            int consumePutEvent(const std::string& key,
                                const uint8_t hst_tag,
                                const ChaosStringSetConstSPtr meta_tag_set,
                                chaos::common::data::BufferSPtr channel_data);
            
            int consumeHealthDataEvent(const std::string& key,
                                       const uint8_t hst_tag,
                                       const ChaosStringSetConstSPtr meta_tag_set,
                                       chaos::common::data::BufferSPtr channel_data);
            
        public:
            QueryDataMsgPSConsumer():msgbrokerdrv("kafka-rdk"),msgbroker("localhost:9092"),groupid("cds") {
                 
  if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_HA_ZONE_NAME)) {
    groupid = groupid + "_" + GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_HA_ZONE_NAME);
  }

  if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_MSG_BROKER_DRIVER)) {
    msgbrokerdrv = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_MSG_BROKER_DRIVER);
  }
  cons = chaos::common::message::MessagePSDriver::getConsumerDriver(msgbrokerdrv, groupid);


            }
            ~QueryDataMsgPSConsumer();
            void init(void *init_data);
            void start();
            void stop();
            void deinit();
            
        };
    }
}

#endif /* defined(__CHAOSFramework__QueryDataMsgPSConsumer__) */
