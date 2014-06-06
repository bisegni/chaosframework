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
#include "index_system/StageDataVFileScanner.h"

#include <chaos/common/utility/ObjectSlot.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/TemplatedKeyValueHash.h>
#include <chaos/common/async_central/AsyncCentralManager.h>

#include <vector>

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lockfree/queue.hpp>
namespace chaos{
    namespace data_service {
        
        class ChaosDataService;
		
		/*!
		 This struct permit to help the usage of different scanner
		 with the usage of some thread. Only one thread a time
		 can use a scanner. Every call to the scan methdo of the
		 StageDataVFileScanner class permit to scan an intere data block
		 */
		typedef struct StageScannerInfo {
			//the sequencial index of the scanner
			uint32_t							index;
			
			//keep track if this scanner is in use by another thread
			bool								in_use;
			
			//schedule the reading and setting of the "in_use" var
			boost::mutex						mutex_on_scan;
			
			//is the scanner for this slot
			index_system::StageDataVFileScanner	*scanner;
		}StageScannerInfo;

		/*!
		  Worker for stage data elaboration
		 */
        class StageDataConsumer :
		public utility::StartableService,
		protected chaos::common::async_central::TimerHanlder {
            friend class ChaosDataService;
			ChaosDataServiceSetting	*settings;
			vfs::VFSManager *vfs_manager_instance;
			chaos::common::utility::ObjectSlot<chaos::data_service::worker::DataWorker*> indexer_stage_worker_list;

			//thread managment
			bool work_on_stage;
			
			//! track the global number of the scanner that we have loaded
			uint32_t global_scanner_num;
			
			//! kee track for the foundand in working path
			std::vector<std::string> vector_working_path;
			
			//gourp for all thread
			boost::thread_group thread_scanners;
			//! this queue is used to scehdule the work on all scanner
			//! and permit without lock to fiil the wueue with new scanner
			//! when they are available
			boost::lockfree::queue<StageScannerInfo*, boost::lockfree::fixed_sized<false> > queue_scanners;
			//scanner thread entry point
			void scanStage();
			
			//async central timer hook
			void timeout();
			
			StageScannerInfo *getNextAvailableScanner();
			
			void rescheduleScannerInfo(StageScannerInfo *scanner_info);
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
