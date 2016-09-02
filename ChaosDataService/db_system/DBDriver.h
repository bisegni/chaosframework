/*
 *	DBDriver.h
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

#ifndef __CHAOSFramework__DBDriver__
#define __CHAOSFramework__DBDriver__

#include <map>
#include <string>
#include <vector>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

#include "db_system_types.h"

namespace chaos {
	namespace data_service {
		
		namespace vfs {
			class VFSFile;
			struct DataBlock;
		}
		namespace chaos_vfs = chaos::data_service::vfs;
		
		namespace db_system {
			/*!
			 Base class for all driver that will manage the work on index database.
			 The base role of the index driver is to help the fast storage
			 of into stage area, coordinate the worker that migrate data between stage and data area
			 of the !CHAOS virtual filesystem. At the end it will help to achive the execution
			 of query on memoryzed data
			 */
			class DBDriver:
			public common::utility::NamedService,
			public common::utility::InizializableService {
			protected:
				//protected constructor
				DBDriver(std::string alias);
			protected:
				DBDriverSetting *setting;
				
			public:
				
				//! public destructor
				virtual ~DBDriver();
				
				//! init
				/*!
				 Need a point to a structure DBDriverSetting for the setting
				 */
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
								
				//! Create a new snapshot
				/*!
				 Create a new snapshot with the name
				 \param snapshot_name the name of the new snapshot
				 \param the new job identification id, subseguent operation on snapshot need to be done using this code, otherwise
				 they will fails.
				 */
				virtual int snapshotCreateNewWithName(const std::string& snapshot_name,
													  std::string& working_job_unique_id) = 0;
				
				//! Add an element to a named snapshot
				/*!
				 add an element to the snapshot
				 \param working_job_unique_id the identification of the job
				 \param snapshot_name the name of the snapshot where put the element
				 \param producer_unique_key the unique key of the producer
				 \param dataset_type the type of the dataset, refer to @DataPackPrefixID field of the dataset
				 \param data the serialized data of the dataset
				 \param data_len the length of the serialized data
				 */
				virtual int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
														 const std::string& snapshot_name,
														 const std::string& producer_unique_key,
														 const std::string& dataset_type,
														 void* data,
														 uint32_t data_len) = 0;
				
				//! Increment or decrement the number of the job that are working on
				/*!
				 this function permit to manage the number of job that are working on the snapshot
				 \param working_job_unique_id the code associated to the job
				 \param snapshot_name the name of the snapshot to modify
				 \param add if true it add +1 if false add -1
				 */
				virtual int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
														const std::string& snapshot_name,
														bool add) = 0;
				
				//! get the dataset from a snapshot
				/*!
				 Return the dataset asociated to a prducer key from a determinated
				 snapshot
				 \param snapshot_name the name of the snapshot to delete
				 \param producer_unique_key the unique key of the producer
				 \param dataset_type the type of the dataset, refer to @DataPackPrefixID field of the dataset
				 \param channel_data the data of the channel;
				 \param channel_data_size the size of the channel data
				 */
				virtual int snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
															 const std::string& producer_unique_key,
															 const std::string& dataset_type,
															 void **channel_data,
															 uint32_t& channel_data_size) = 0;
				
				//! Delete a snapshot where no job is working
				/*!
				 Delete the snapshot and all dataset associated to it
				 \param snapshot_name the name of the snapshot to delete
				 */
				virtual int snapshotDeleteWithName(const std::string& snapshot_name) = 0;

			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DBDriver__) */
