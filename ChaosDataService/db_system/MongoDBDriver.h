/*
 *	MongoDBDriver.h
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
#ifndef __CHAOSFramework__MongoDBDriver__
#define __CHAOSFramework__MongoDBDriver__

#include "DBDriver.h"
#include "mongo_db_types.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

namespace chaos {
	namespace data_service {
			namespace db_system {
				class MongoDBIndexCursor;
				
				//! Mongodb implementation for the index driver
				DECLARE_CLASS_FACTORY(MongoDBDriver, DBDriver) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBDriver)
					MongoDBDriver(std::string alias);
					friend class MongoDBIndexCursor;
					std::string db_name;
				protected:
					//! ha mongodb driver
					service_common::persistence::mongodb::MongoDBHAConnectionManager *ha_connection_pool;
					
				public:
					~MongoDBDriver();
					
					//! init
					void init(void *init_data) throw (chaos::CException);
					
					//!deinit
					void deinit() throw (chaos::CException);
					
					//! Create a new snapshot
					int snapshotCreateNewWithName(const std::string& snapshot_name,
												  std::string& working_job_unique_id);
					
					//! Add an element to a named snapshot
					int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
													 const std::string& snapshot_name,
													 const std::string& producer_unique_key,
													 const std::string& dataset_type,
													 void* data,
													 uint32_t data_len);
					
					//! Increment or decrement the number of the job that are working on
					int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
													const std::string& snapshot_name,
													bool add);
					
					//! get the dataset from a snapshot
					virtual int snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
																 const std::string& producer_unique_key,
																 const std::string& dataset_type,
																 void **channel_data,
																 uint32_t& channel_data_size);
					
					//! Delete a snapshot where no job is working
					virtual int snapshotDeleteWithName(const std::string& snapshot_name);
				};
			}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBDriver__) */
