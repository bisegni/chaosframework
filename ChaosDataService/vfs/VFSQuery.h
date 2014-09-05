/*
 *	VFSQuery.h
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

#ifndef __CHAOSFramework__VFSQuerableDataFile__
#define __CHAOSFramework__VFSQuerableDataFile__

#include "VFSDataFile.h"

#include "../db_system/DBDriver.h"
#include "storage_system/StorageDriver.h"

#include <vector>
#include <memory>

namespace chaos {
	namespace data_service {
		namespace vfs {
			//forward declaration
			class VFSManager;
			
			//! Data querable file
			/*!
			 This class permit to perform a query on the a virtual data file and
			 reading the result in single or array element.
			 */
			class VFSQuery {
				friend class VFSManager;
				
				//!index driver pointer
				chaos_index::DBDriver *db_driver_ptr;
				
				//!storage driver pointer
				storage_system::StorageDriver *storage_driver_ptr;
				
				//! query on virtual file system to perform
				chaos::data_service::db_system::DataPackIndexQuery query;
				
				
				auto_ptr<chaos::data_service::db_system::DBIndexCursor> query_cursor_ptr;
				//!contructor for a timed query
				/*!
				 Construct a data reading file with a timed query adn ordering
				 \param data_vfs_relative_path virtual file path
				 \param start_ts the start time stamp of the first needed data pack
				 \param end_ts the end timestamp of the last neede datapack
				 \param asc is the order of the resulting packet
				 */
				VFSQuery(storage_system::StorageDriver *_storage_driver_ptr,
						 db_system::DBDriver *_db_driver_ptr,
						 const chaos::data_service::db_system::DataPackIndexQuery& _query);
				
				
				//! load data block containing index
				int getDatablockForIndex(const db_system::DataPackIndexQueryResult& index, vfs::DataBlock **datablock_ptr);
				
				//! return a datapack for the index
				int getDataPackForIndex(const db_system::DataPackIndexQueryResult& index);
			public:
				
				//! ensure that a datablock is not null
				/*!
				 usefullt to get the current lcoation before write the first data pack.
				 */
				int executeQuery();
				
				// read a single query result
				int nextDataPack(void **data);
				
				// read a bunch of result data
				int nextNDataPack(std::vector<void*> &readed_pack, int to_read);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSQuerableDataFile__) */
