/*
 *	IndexStageDataWorker.h
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
#ifndef __CHAOSFramework__IndexStageDataWorker__
#define __CHAOSFramework__IndexStageDataWorker__

#include "DataWorker.h"
#include "../vfs/VFSManager.h"
#include "../vfs/VFSFile.h"
#include "../cache_system/cache_system.h"
namespace chaos{
    namespace data_service {
		namespace worker {
			
			struct IndexStageDataJob : public WorkerJob {
				
			};
			
			class IndexStageDataWorker : public DataWorker {
			protected:
				vfs::VFSManager *_vfs_manager_instance;
				void executeJob(WorkerJobPtr job_info, void* cookie);
			public:
				IndexStageDataWorker(vfs::VFSManager *_vfs_manager_instance);
				~IndexStageDataWorker();
				void init(void *init_data) throw (chaos::CException);
				void deinit() throw (chaos::CException);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__IndexStageDataWorker__) */
