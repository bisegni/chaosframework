/*
 *	DataBlockCache.cpp
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


#include "DataBlockCache.h"

#define DataBlockCache_CLEANUP_TIMER_TIME 25000


using namespace chaos::data_service::vfs::query;

#define DBC_LOG_HEAD "[DataBlockCache] - "
#define DBC_LAPP_ LAPP_ << DBC_LOG_HEAD
#define DBC_LDBG_ LDBG_ << DBC_LOG_HEAD << __FUNCTION__ << " - "
#define DBC_LERR_ LERR_ << DBC_LOG_HEAD << __FUNCTION__ << " - " << __LINE__


//! Initialize instance
void DataBlockCache::init(void *init_data) throw(chaos::CException) {
	if(!storage_driver) throw chaos::CException(-1, "No storage driver found", __PRETTY_FUNCTION__);
	chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, 0, DataBlockCache_CLEANUP_TIMER_TIME);

}

//! Deinit the implementation
void DataBlockCache::deinit() throw(chaos::CException) {
	chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);
	
	//clean all
	clean(true);
}

// TimerHandler callback
void DataBlockCache::timeout() {
	//clan only unused
	clean(false);
}

//clean all or aged (timed out) data block fetcher
void DataBlockCache::clean(bool all_unused) {
	boost::unique_lock<boost::shared_mutex> wmap_lock(map_path_datablock_mutex);

	for (MapPathDatablockIterator it = map_path_datablock.begin();
		 it != map_path_datablock.end();) {
		if(!it->second->use_count || all_unused) {
			DBC_LDBG_ << "Deleting datablock fetcher for " << it->first;
			//no one is using the fetcher or all fetcher need to be claned
			delete(it->second);
			//the right auto increment return the previus element
			map_path_datablock.erase(it++);
		} else {
			//fetcher is still in use so i step forward to the next
			++it; //more performant
		}
	}
}

//! Get a new fetcher for datablock
int DataBlockCache::getFetcherForBlockWithPath(const std::string& datablock_path,
											   DataBlockFetcher **fetcher) {
	int err = 0;
	boost::upgrade_lock<boost::shared_mutex> rmap_lock(map_path_datablock_mutex);
	MapPathDatablockIterator db_iter = map_path_datablock.find(datablock_path);
	if(db_iter != map_path_datablock.end()) {
		DBC_LDBG_ << "return already allocated datablock fetcher for " << datablock_path;
		
		//return mapped datablock
		*fetcher = db_iter->second;
		
		(*fetcher)->use_count++;
	} else {
		DBC_LDBG_ << "Allocating new datablock fetcher for " << datablock_path;
		
		//open new datablock fetcher
		boost::upgrade_to_unique_lock<boost::shared_mutex> wmap_lock(rmap_lock);
		
		//install datablock fetcher in map
		*fetcher = new DataBlockFetcher(storage_driver, datablock_path);
		if(!(*fetcher) ||
		   (err = (*fetcher)->open())) {
			//error
		} else {
			//all is gone well and we can put the fetcher in hashmap
			map_path_datablock.insert(make_pair(datablock_path, *fetcher));
			
			(*fetcher)->use_count++;
		}
	}
	return err;
}

//! Release a previously allocated fetcher for datablock
int DataBlockCache::releaseFetcher(DataBlockFetcher *datablock_fetcher) {
	int err = 0;
	if(!datablock_fetcher) return 0;
	datablock_fetcher->use_count--;
	return err;
}