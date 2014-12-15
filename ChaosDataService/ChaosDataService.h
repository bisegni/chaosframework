/*
 *	ChaosDataService.h
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

#ifndef __CHAOSFramework__ChaosDataService__
#define __CHAOSFramework__ChaosDataService__

#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "dataservice_global.h"
#include "QueryDataConsumer.h"
#include "StageDataConsumer.h"
#include "worker/DataWorker.h"
#include "cache_system/CacheDriver.h"
#include "vfs/vfs.h"
#include "db_system/db_system.h"
#include <map>
#include <string>
#include <vector>

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>


namespace common_utility = chaos::utility;

namespace chaos{
    namespace data_service {

		typedef enum RunMode {
			QUERY = 1,
			INDEXER = 2,
			BOTH = 3
		} RunMode;
		
        //! Chaos Data Service singleton
        /*!
         */
        class ChaosDataService : public ChaosCommon<ChaosDataService> {
            friend class Singleton<ChaosDataService>;
            
			RunMode run_mode;
            static WaitSemaphore waitCloseSemaphore;
            
            ChaosDataService();
            ~ChaosDataService();
            static void signalHanlder(int);
			
			//root pointer of the index driver
			db_system::DBDriver *db_driver_ptr;
			
			
			//!chaos internal network router
			utility::StartableServiceContainer<chaos::common::network::NetworkBroker> network_broker;
			
			//! CDS virtual file system manager
			utility::InizializableServiceContainer<vfs::VFSManager> vfs_file_manager;

			//! CDS data consumer that respond to data api
			utility::StartableServiceContainer<QueryDataConsumer> data_consumer;
			
			//! CDS index part that elaborate stage file
			utility::StartableServiceContainer<StageDataConsumer> stage_data_consumer;
			
			//! convert param_key to a string of string hash map
			void fillKVParameter(std::map<std::string, std::string>& kvmap, const char * param_key);
        public:
			
			ChaosDataServiceSetting setting;
			
            //! C and C++ attribute parser
            /*!
             Specialized option for startup c and cpp program main options parameter
             */
            void init(int argc, char* argv[]) throw (CException);
            //!stringbuffer parser
            /*
             specialized option for string stream buffer with boost semantics
             */
            void init(istringstream &initStringStream) throw (CException);
            void init(void *init_data)  throw(CException);
            void start() throw(CException);
            void stop() throw(CException);
            void deinit() throw(CException);
        };
    }
}

#endif /* defined(__CHAOSFramework__ChaosDataProxy__) */
