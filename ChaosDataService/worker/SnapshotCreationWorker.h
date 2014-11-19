/*
 *	SnapshotCreationWorker.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__SnapshootCreationWorker__
#define __CHAOSFramework__SnapshootCreationWorker__

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
			namespace chaos_direct_io	= chaos::common::direct_io;
			

			//! worker information for snapshot creation
			struct SnapshotCreationJob :
			public WorkerJob {
				//the name of the new snapshot
				std::string					snapshot_name;
				
				//! if empty all key need to be insert into the snapshot
				std::vector<std::string>	produceter_unique_id_set;
			};
			
			//! worker for create the snapshoot in async way
			class SnapshotCreationWorker:
			public DataWorker {
				std::string cache_impl_name;
				std::string db_impl_name;
				
				cache_system::CacheDriver	*cache_driver_ptr;
				db_system::DBDriver			*db_driver_ptr;
				
				//channel to mds
				//MDSMessageChannel *mds_message_channel;
			protected:
				void executeJob(WorkerJobPtr job_info, void* cookie);
			public:
				SnapshotCreationWorker(const std::string& _cache_impl_name,
									   const std::string& _db_impl_name);
				~SnapshotCreationWorker();
				void init(void *init_data) throw (chaos::CException);
				void deinit() throw (chaos::CException);
				void addServer(std::string server_description);
				void updateServerConfiguration();
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__SnapshootCreationWorker__) */
