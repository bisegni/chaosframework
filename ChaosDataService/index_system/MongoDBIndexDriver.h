/*
 *	MongoDBIndexDriver.h
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
#ifndef __CHAOSFramework__MongoDBIndexDriver__
#define __CHAOSFramework__MongoDBIndexDriver__

#include "IndexDriver.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include "mongo/client/dbclient.h"

namespace chaos {
	namespace data_service {
		namespace index_system {
			
			//! Mongodb implementation for the index driver
			REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(MongoDBIndexDriver, IndexDriver) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBIndexDriver)
				mongo::ConnectionString connection_string;
				MongoDBIndexDriver(std::string alias);
			protected:
				mongo::DBClientConnection mongo_client;
				
			public:
				~MongoDBIndexDriver();
				
				//! init
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
				
				//! Register a new data block wrote on stage area
				int addNewStageDataBlock(chaos_vfs::DataBlock *data_block);
				
				//! Set the state for a stage datablock
				int setStateOnStageDataBlock(chaos_vfs::DataBlock *data_block, StageDataBlockState state);
				
				//! Heartbeat update stage block
				int workHeartBeatOnStageDataBlock(chaos_vfs::DataBlock *data_block);
				
				
				//! Retrive the path for all datablock in a determinate state
				int getStageDataBlockPathByState(std::vector<std::string>& path_data_block, StageDataBlockState state);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBIndexDriver__) */
