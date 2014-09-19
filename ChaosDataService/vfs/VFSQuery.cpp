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
#include "query/DataBlockCache.h"

using namespace chaos::data_service::vfs;

#define VFSQuery_LOG_HEAD "[VFSQuery] - "
#define VFSQ_LAPP_ LAPP_ << VFSQuery_LOG_HEAD
#define VFSQ_LDBG_ LDBG_ << VFSQuery_LOG_HEAD << __FUNCTION__ << " - "
#define VFSQ_LERR_ LERR_ << VFSQuery_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

#define DataPackIndexQueryResult_PRINT_INFO(x)\
"domain:" << index.dst_location->getBlockVFSDomain() <<\
" path:"<< index.dst_location->getBlockVFSPath()
//!contructor for a timed query
VFSQuery::VFSQuery(storage_system::StorageDriver *_storage_driver_ptr,
				   db_system::DBDriver *_db_driver_ptr,
				   const chaos::data_service::db_system::DataPackIndexQuery& _query):
storage_driver_ptr(_storage_driver_ptr),
db_driver_ptr(_db_driver_ptr),
query(_query),
fetched_element(0) {
	
}

VFSQuery::~VFSQuery() {
	for(MapPathDatablockIterator it =  map_path_datablock.begin();
		it != map_path_datablock.end();
		it++) {
		// release all fetcher
		VFSQ_LAPP_ << "Release datablock fetcher for " << it->first;
		query::DataBlockCache::getInstance()->releaseFetcher(it->second);
	}
}

//! load data block containing index
int VFSQuery::getDatablockFetcherForIndex(const db_system::DataPackIndexQueryResult& index,
										  query::DataBlockFetcher **datablock_ptr) {
	int err = 0;
	std::string datablock_path = index.dst_location->getBlockVFSPath();
	MapPathDatablockIterator db_iter = map_path_datablock.find(datablock_path);
	if(db_iter != map_path_datablock.end()) {
		//return mapped datablock
		*datablock_ptr = db_iter->second;
	} else if(!(err = query::DataBlockCache::getInstance()->getFetcherForBlockWithPath(datablock_path, datablock_ptr))) {
			//all is gone well and we can put the fetcher in hashmap
		map_path_datablock.insert(make_pair(datablock_path, *datablock_ptr));
	}
	return err;
}

//! return a datapack for the index
int VFSQuery::getDataPackForIndex(const db_system::DataPackIndexQueryResult& index, void** data, uint32_t& data_len) {
	int err = 0;
	query::DataBlockFetcher *db_fetcher = NULL;
	if((err = getDatablockFetcherForIndex(index, &db_fetcher))){
		VFSQ_LERR_ << "Error retriving the datablock fetcher for path:" << DataPackIndexQueryResult_PRINT_INFO(index);
	} else if(!db_fetcher) {
		VFSQ_LERR_ << "no error and no datablock fetcher for " << DataPackIndexQueryResult_PRINT_INFO(index);
	} else {
		//we have the fetcher
		if((err = db_fetcher->readData(index.dst_location->getOffset(), (data_len = index.datapack_size), data))) {
			VFSQ_LERR_ << "Error fetching data for " << DataPackIndexQueryResult_PRINT_INFO(index);
		}
	}
	return err;
}

//! ensure that a datablock is not null
/*!
 usefullt to get the current allocation before write the first data pack.
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

// read a bunch of result data
int VFSQuery::readDataPackPage(std::vector<FoundDataPack*> &readed_pack) {
	int err = 0;
	void *data = NULL;
	
	if(query_cursor_ptr->getCurrentPage()==query_cursor_ptr->getTotalPage()) return 0;
	
	if((err = query_cursor_ptr->performNextPagedQuery())) {
		return err;
	}
	
	uint32_t data_len = 0;
	while (query_cursor_ptr->hasNext()) {
		auto_ptr<db_system::DataPackIndexQueryResult> current_index(query_cursor_ptr->getIndex());
		if((err = getDataPackForIndex(*current_index.get(), &data, data_len))){
			VFSQ_LERR_ << "Error retriving the data pack on virtual filesystem";
		}else {
			readed_pack.push_back(new FoundDataPack(data, data_len));
			data = NULL;
			data_len = 0;
		}
	}
	//update the fetche element number
	fetched_element += readed_pack.size();
	return err;
}

uint64_t VFSQuery::getNumberOfElementFound() {
	return query_cursor_ptr->getNumberOfElementFound();
}

uint64_t VFSQuery::getNumberOfFetchedElement() {
	return fetched_element;
}