/*
 * Copyright 2012, 2017 INFN
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

#ifndef __CHAOSFramework__QueryDataConsumer__
#define __CHAOSFramework__QueryDataConsumer__

#include "mds_types.h"
#include "worker/DataWorker.h"
#include "cache_system/cache_system.h"

#include <chaos/common/utility/ObjectSlot.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/NetworkBroker.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

namespace chaos{
    namespace data_service {
        
        class ChaosDataService;
        
        class QueryDataConsumer:
        protected DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler,
        protected DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler,
        public StartableService {
            friend class ChaosDataService;
            
            std::string cache_impl_name;
            std::string db_impl_name;
            service_common::persistence::data_access::AbstractPersistenceDriver *object_storage_driver;
            DirectIOServerEndpoint					*server_endpoint;
            DirectIODeviceServerChannel				*device_channel;
            DirectIOSystemAPIServerChannel			*system_api_channel;
            
            
            boost::atomic<uint16_t>                 device_data_worker_index;
            chaos::data_service::worker::DataWorker	**device_data_worker;
            
            //---------------- DirectIODeviceServerChannelHandler -----------------------
            int consumePutEvent(const std::string& key,
                                const uint8_t hst_tag,
                                const ChaosStringSetConstSPtr meta_tag_set,
                                chaos::common::data::BufferSPtr channel_data);
            
            int consumeHealthDataEvent(const std::string& key,
                                       const uint8_t hst_tag,
                                       const ChaosStringSetConstSPtr meta_tag_set,
                                       chaos::common::data::BufferSPtr channel_data);
            
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
                                      const uint64_t search_start_ts,
                                      const uint64_t search_end_ts,
                                      opcode_headers::SearchSequence& last_element_found_seq,
                                      opcode_headers::QueryResultPage& page_element_found);
            
            int consumeDataCloudDelete(const std::string& search_key,
                                       uint64_t start_ts,
                                       uint64_t end_ts);
            
            //---------------- DirectIOSystemAPIServerChannelHandler -----------------------
            int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader& header,
                                               const std::string& producer_id,
                                               chaos::common::data::BufferSPtr& channel_found_data,
                                               DirectIOSystemAPISnapshotResultHeader &result_header);
            
            int consumeLogEntries(const std::string& node_name,
                                  const ChaosStringVector& log_entries);
        public:
            QueryDataConsumer();
            ~QueryDataConsumer();
            void init(void *init_data);
            void start();
            void stop();
            void deinit();
            
        };
    }
}

#endif /* defined(__CHAOSFramework__QueryDataConsumer__) */
