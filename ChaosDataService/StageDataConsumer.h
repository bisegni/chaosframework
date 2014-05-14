/*
 *	StageDataConsumer.h
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
#ifndef __CHAOSFramework__StageDataConsumer__
#define __CHAOSFramework__StageDataConsumer__

#include "dataservice_global.h"
#include "vfs/VFSManager.h"
#include "worker/DataWorker.h"
#include "cache_system/cache_system.h"
#include <chaos/common/utility/ObjectSlot.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/async_central/async_central.h>

#include <vector>
namespace chaos{
    namespace data_service {
        
        class ChaosDataService;
		
		/*!
		  Worker for stage data elaboration
		 */
        class StageDataConsumer : public utility::StartableService {
            friend class ChaosDataService;
			ChaosDataServiceSetting	*settings;
			vfs::VFSManager *vfs_manager_instance;
			chaos::common::utility::ObjectSlot<chaos::data_service::worker::DataWorker*> indexer_stage_worker_list;

			bool work_on_stage;
			utility::delegate::Delegate myself_delegate;
			std::vector<common::async_central::AcmThreadID> thread_ids;
			
			void scanStage();
		public:
			StageDataConsumer(vfs::VFSManager *_vfs_manager_instance, ChaosDataServiceSetting *_settings);
            ~StageDataConsumer();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);
		};
	}
}


#endif /* defined(__CHAOSFramework__StageDataConsumer__) */
