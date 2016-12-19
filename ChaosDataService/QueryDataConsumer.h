/*
 *	QueryDataConsumer.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__QueryDataConsumer__
#define __CHAOSFramework__QueryDataConsumer__

#include "dataservice_global.h"
#include "db_system/DBDriver.h"
#include "worker/DataWorker.h"
#include "cache_system/cache_system.h"
#include "db_system/db_system.h"

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
			
			NetworkBroker							*network_broker;
            db_system::DBDriver                     *db_driver;
            service_common::persistence::data_access::AbstractPersistenceDriver *object_storage_driver;
            DirectIOServerEndpoint					*server_endpoint;
			DirectIODeviceServerChannel				*device_channel;
			DirectIOSystemAPIServerChannel			*system_api_channel;
			
            
			boost::atomic<uint16_t>                 device_data_worker_index;
			chaos::data_service::worker::DataWorker	**device_data_worker;
			chaos::data_service::worker::DataWorker	*snapshot_data_worker;
			
			//---------------- DirectIODeviceServerChannelHandler -----------------------
            int consumePutEvent(DirectIODeviceChannelHeaderPutOpcode *header,
								void *channel_data,
								uint32_t channel_data_len);
			
            int consumeHealthDataEvent(DirectIODeviceChannelHeaderPutOpcode *header,
                                       void *channel_data,
                                       uint32_t channel_data_len);
            
            int consumeGetEvent(DirectIODeviceChannelHeaderGetOpcode *header,
								void *channel_data,
								uint32_t channel_data_len,
                                opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                void **result_value);
			
            int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud *query_header,
                                      const std::string& search_key,
                                      uint64_t search_start_ts,
                                      uint64_t search_end_ts,
                                      uint64_t last_sequence_id,
                                      opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult * result_header,
                                      void **result_value);
			
            int consumeDataCloudDelete(const std::string& search_key,
                                       uint64_t start_ts,
                                       uint64_t end_ts);
            
			//---------------- DirectIOSystemAPIServerChannelHandler -----------------------
			// Manage the creation of a snapshot
			int consumeNewSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
										void *concatenated_unique_id_memory,
										uint32_t concatenated_unique_id_memory_size,
										DirectIOSystemAPISnapshotResultHeader& api_result);
			
			// Manage the delete operation on an existing snapshot
			int consumeDeleteSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
										   DirectIOSystemAPISnapshotResultHeader& api_result);
			
			// Return the dataset for a producerkey ona specific snapshot
			int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
											   const std::string& producer_id,
											   void **channel_found_data,
											   uint32_t& channel_found_data_length,
											   DirectIOSystemAPISnapshotResultHeader& api_result);
        public:
			QueryDataConsumer();
            ~QueryDataConsumer();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);

        };
    }
}

#endif /* defined(__CHAOSFramework__QueryDataConsumer__) */
