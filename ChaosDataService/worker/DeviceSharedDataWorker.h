/*
 *	DeviceDataWorker.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DeviceDataWorker__
#define __CHAOSFramework__DeviceDataWorker__

#include "DataWorker.h"
#include "../cache_system/cache_system.h"

#include <string>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
namespace chaos_direct_io = chaos::common::direct_io;

namespace chaos{
    namespace data_service {
		namespace worker {
			
			//!worker information for the device live storage
			struct DeviceSharedWorkerJob : public WorkerJob {
				common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *request_header;
				uint32_t data_pack_len;
				void * data_pack;
			};
			
			
			//! worker for live device sharing
			class DeviceSharedDataWorker : public DataWorker {
				cache_system::CacheDriver *cache_driver_instance;
			protected:
				void executeJob(WorkerJobPtr job_info);
				
			public:
				DeviceSharedDataWorker(std::string cache_impl_name);
				~DeviceSharedDataWorker();
				
				void addServer(std::string server_description);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DeviceDataWorker__) */
