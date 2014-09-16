/*
 *	MongoDBIndexCursor.cpp
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

#include "MongoDBDriver.h"
#include "MongoDBIndexCursor.h"

#include <math.h>

using namespace chaos::data_service::db_system;

//! private constructor
MongoDBIndexCursor::MongoDBIndexCursor(DBDriver *_driver_ptr,
									   const DataPackIndexQuery& _query):
DBIndexCursor(_driver_ptr, _query),
time_offset_per_page_in_ms(0),
last_max_ts_searched(0) {
}

MongoDBIndexCursor::~MongoDBIndexCursor() {
}

int MongoDBIndexCursor::computeTimeLapsForPage() {
	int err = 0;
	MongoDBDriver *mdrv = static_cast<MongoDBDriver*>(driver_ptr);
	
	//check if max and min timestamp has been gived
	if(!query.start_ts || !query.end_ts) {
		uint64_t max_ts;
		uint64_t min_ts;
		err = mdrv->idxMaxAndMInimumTimeStampForDataPack(query, min_ts, max_ts);
		if(err) return err;
		
		if(!query.start_ts) query.start_ts = min_ts;
		if(!query.end_ts) query.end_ts = max_ts;
	}
	
	//calculate the pagin information
	err = mdrv->idxSearchResultCountDataPack(query, element_found);
	if(!err) {
		//!calculate total element for the time laps request
		total_page_number = (uint32_t)ceil((double)element_found / (double)getResultPageDimension());
		
		//!calculate the laps of the single page
		time_offset_per_page_in_ms = (uint64_t)ceil((double)((query.end_ts - query.start_ts)/total_page_number));
	}
	return err;
}

int MongoDBIndexCursor::performNextPagedQuery() {
	MongoDBDriver *mdrv = static_cast<MongoDBDriver*>(driver_ptr);
	
	//advance page indicator
	current_page++;
	
	//check if we have terminated the query page
	if(last_max_ts_searched >= query.end_ts) return 0;
	
	DataPackIndexQuery paged_query;
	paged_query.did = query.did;
	//set the start timestamp
	paged_query.start_ts = (last_max_ts_searched?last_max_ts_searched+1:query.start_ts);
	
	//set the end timestamp
	last_max_ts_searched = paged_query.end_ts = (paged_query.start_ts + time_offset_per_page_in_ms);
	
	//perform query
	return mdrv->idxSearchDataPack(paged_query, cursor);
}

//! return true if there are othere index to fetch
bool MongoDBIndexCursor::hasNext() {
	return (cursor.get() == NULL)? false:cursor->more();
}

//! return next index
DataPackIndexQueryResult *MongoDBIndexCursor::getIndex() {
	mongo::BSONObj next_element = cursor->next();
	DataPackIndexQueryResult *result = new DataPackIndexQueryResult();
	std::string block_domain = next_element.getField(MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN).String();
	std::string block_path = next_element.getField(MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_PATH).String();
	uint64_t block_offset = (uint64_t)next_element.getField(MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET).Long();
	
	result->dst_location = new chaos::data_service::vfs::PathFileLocation(block_domain, block_path, block_offset);
	result->datapack_size = (uint32_t)next_element.getField(MONGO_DB_IDX_DATA_PACK_SIZE).numberInt();
	return result;
}