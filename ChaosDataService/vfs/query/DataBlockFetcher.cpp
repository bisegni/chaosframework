/*
 *	DataBlockFetcher.cpp
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


#include "DataBlockFetcher.h"
#include <chaos/common/global.h>
using namespace chaos::data_service::vfs::query;

#define DBF_LOG_HEAD "[DataBlockFetcher] - " << datablock_path << " - "
#define DBF_LAPP_ LAPP_ << DBF_LOG_HEAD
#define DBF_LDBG_ LDBG_ << DBF_LOG_HEAD << __FUNCTION__ << " - "
#define DBF_LERR_ LERR_ << DBF_LOG_HEAD << __FUNCTION__ << " - " << __LINE__ << " "


DataBlockFetcher::DataBlockFetcher(storage_system::StorageDriver *_storage_driver,
								   const std::string & _datablock_path):
use_count(0),
storage_driver(_storage_driver),
datablock_path(_datablock_path),
datablock(NULL) {
	
}

DataBlockFetcher::~DataBlockFetcher() {
	close();
}

int DataBlockFetcher::open() {
	int err = 0;
	//open the datablock file
	if((err = storage_driver->openBlock(datablock_path, block_flag::BlockFlagReadeable, &datablock))) {
		DBF_LERR_ << "Error openign datablock";
	}
	return err;
}

int DataBlockFetcher::close() {
	int err = 0;
	if(!datablock) return -1;
	//close the data block file
	if((err = storage_driver->closeBlock(datablock))) {
		DBF_LERR_ << "Error openign datablock";
	}
	datablock = NULL;
	return err;
}

//read data
int DataBlockFetcher::readData(uint64_t offset, uint32_t data_len, void **data_handler) {
	int err = 0;
	boost::unique_lock<boost::mutex> rlock(mutex_read_access);
	
	if(offset >= datablock->max_reacheable_size) {
		err = -1000; //off
		DBF_LERR_ << "Offset outside file " << err;
	} else if((err = storage_driver->seekg(datablock, offset, block_seek_base::BlockSeekBaseBegin))) {
		DBF_LERR_ << "Error going to offset " << err;
	} else {
		//we can read so we need to allcoate memory for reading
		uint32_t readed_byte;
		*data_handler = calloc(data_len, 1);
		if((err = storage_driver->read(datablock, *data_handler, data_len, readed_byte))) {
			if(*data_handler) free(*data_handler);
			DBF_LERR_ << "Error reading data " << err;
		} else {
			//we got data
		}
	}
	return err;
}
