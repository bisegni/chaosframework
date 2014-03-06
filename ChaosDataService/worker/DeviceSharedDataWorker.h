//
//  DeviceDataWorker.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DeviceDataWorker__
#define __CHAOSFramework__DeviceDataWorker__

#include "DataWorker.h"
#include "../cache_system/cache_system.h"

#include <string>
#include <chaos/common/direct_io/DirectIODataPack.h>

namespace chaos_direct_io = chaos::common::direct_io;

namespace chaos{
    namespace data_service {
		namespace worker {
			
			//!worker information for the device live storage
			struct DeviceSharedWorkerJob : public WorkerJob {
				uint32_t device_hash;
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
