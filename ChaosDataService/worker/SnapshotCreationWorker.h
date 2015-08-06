/*
 *	SnapshotCreationWorker.h
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

#ifndef __CHAOSFramework__SnapshotCreationWorker__
#define __CHAOSFramework__SnapshotCreationWorker__

#include "DataWorker.h"
#include "../db_system/db_system.h"
#include "../cache_system/cache_system.h"

#include <string>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/message/MDSMessageChannel.h>

#include <boost/thread.hpp>

namespace chaos{
	namespace data_service {
		namespace worker {

			//! worker information for snapshot creation
			struct SnapshotCreationJob :
			public WorkerJob {
				//!used to recognize the job that create the snapshot
				std::string		job_work_code;
				
				//!the name of the new snapshot
				std::string		snapshot_name;
				
				//! buffer fileld with concatenated unique id
				char *			concatenated_unique_id_memory;
				
				//the length of the buffer
				uint32_t		concatenated_unique_id_memory_size;
			};
			
			//! worker for create the Snapshot in async way
			class SnapshotCreationWorker:
			public DataWorker {
				//! implementaiton for caching
				std::string cache_impl_name;
				//! implementation for database
				std::string db_impl_name;
				
				//! Network infrastructure
				chaos::common::network::NetworkBroker		*network_broker;
				chaos::common::message::MDSMessageChannel	*mds_channel;
				
				//!
				cache_system::CacheDriver			*cache_driver_ptr;
				db_system::DBDriver					*db_driver_ptr;
				
				int storeDatasetTypeInSnapsnot(const std::string& job_work_code,
											   const std::string& snapshot_name,
											   const std::string& unique_id,
											   const std::string& dataset_type);
			protected:
				void executeJob(WorkerJobPtr job_info, void* cookie);
			public:
				SnapshotCreationWorker(const std::string& _cache_impl_name,
									   db_system::DBDriver	*_db_driver_ptr,
									   chaos::common::network::NetworkBroker	*_network_broker);
				~SnapshotCreationWorker();
				void init(void *init_data) throw (chaos::CException);
				void deinit() throw (chaos::CException);
				int submitJobInfo(WorkerJobPtr job_info);
				void addServer(std::string server_description);
				void updateServerConfiguration();
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__SnapshotCreationWorker__) */
