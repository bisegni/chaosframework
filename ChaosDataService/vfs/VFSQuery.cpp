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

#include "VFSQuery.h"
using namespace chaos::data_service::vfs;

#define VFSQuery_LOG_HEAD "[VFSQuery] - "
#define VFSQ_LAPP_ LAPP_ << VFSQuery_LOG_HEAD
#define VFSQ_LDBG_ LDBG_ << VFSQuery_LOG_HEAD << __FUNCTION__ << " - "
#define VFSQ_LERR_ LERR_ << VFSQuery_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "


//!contructor for a timed query
/*!
 Construct a data reading file with a timed query adn ordering
 \param data_vfs_relative_path virtual file path
 \param start_ts the start time stamp of the first needed data pack
 \param end_ts the end timestamp of the last neede datapack
 \param asc is the order of the resulting packet
 */
VFSQuery::VFSQuery(storage_system::StorageDriver *_storage_driver_ptr,
				   db_system::DBDriver *_db_driver_ptr,
				   const chaos::data_service::db_system::DataPackIndexQuery& _query):
storage_driver_ptr(_storage_driver_ptr),
db_driver_ptr(_db_driver_ptr),
query(_query){

}

//! load data block containing index
int VFSQuery::getDatablockForIndex(const db_system::DataPackIndexQueryResult& index,
								   vfs::DataBlock **datablock_ptr) {
	return 0;
}

//! return a datapack for the index
int VFSQuery::getDataPackForIndex(const db_system::DataPackIndexQueryResult& index) {
	return 0;
}

//! ensure that a datablock is not null
/*!
 usefullt to get the current lcoation before write the first data pack.
 */
int VFSQuery::executeQuery() {
	int err = 0;
	chaos::data_service::db_system::DBIndexCursor *new_cursor;
	if((err = db_driver_ptr->idxStartSearchDataPack(query, &new_cursor))){
		VFSQ_LERR_ << "Error starting query";
		if(new_cursor) delete(new_cursor);
	}else {
		//query performed so reset the
		query_cursor_ptr.reset(new_cursor);
	}
	return err;
}

// read a single query result
int VFSQuery::nextDataPack(void **data) {
	if(query_cursor_ptr->hasNext()) {
		db_system::DataPackIndexQueryResult *current_index = query_cursor_ptr->getIndex();
		
		delete(current_index);
	}
	return 0;
}

// read a bunch of result data
int VFSQuery::nextNDataPack(std::vector<void*> &readed_pack, int to_read) {
	return 0;
}