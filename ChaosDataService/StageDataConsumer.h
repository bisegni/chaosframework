/*
 *	StageDataConsumer.h
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
#ifndef __CHAOSFramework__StageDataConsumer__
#define __CHAOSFramework__StageDataConsumer__

#include "dataservice_global.h"
#include "vfs/VFSManager.h"
#include "worker/DataWorker.h"
#include "indexer/StageDataVFileScanner.h"

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

#include <vector>

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lockfree/queue.hpp>
namespace chaos{
    namespace data_service {
		
		namespace db_system {
			class DBDriver;
		}
		
        class ChaosDataService;

		/*!
		  Worker for stage data elaboration
		 */
        class StageDataConsumer :
		public common::utility::StartableService {
            friend class ChaosDataService;
			ChaosDataServiceSetting	*settings;
			vfs::VFSManager         *vfs_manager_ptr;
			db_system::DBDriver     *db_driver_ptr;

			//thread managment
			bool work_on_stage;
			
			//! keep track of last check of timeouted chunk
			boost::mutex	mutex_timeout_check;
			uint64_t		time_to_check_timeouted_stage_file;

			
			//gourp for all thread
			boost::thread_group thread_scanners;

			//scanner thread entry point
			void scanStage();
			
			//async central timer hook
			void timeout();

		public:
			StageDataConsumer(vfs::VFSManager *_vfs_manager_ptr,
							  db_system::DBDriver *_db_driver_ptr,
							  ChaosDataServiceSetting *_settings);
            ~StageDataConsumer();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);
		};
	}
}


#endif /* defined(__CHAOSFramework__StageDataConsumer__) */
