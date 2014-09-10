//
//  DataBlockFetcher.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 09/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "DataBlockFetcher.h"

using namespace chaos::data_service::vfs::query;

#define DBF_LOG_HEAD "[DataBlockFetcher] - " << data_block_path << " - "
#define DBF_LAPP_ LAPP_ << DBF_LOG_HEAD
#define DBF_LDBG_ LDBG_ << DBF_LOG_HEAD << __FUNCTION__ << " - "
#define DBF_LERR_ LERR_ << DBF_LOG_HEAD << __FUNCTION__ << " - " << __LINE__


DataBlockFetcher::DataBlockFetcher(storage_system::StorageDriver *_storage_driver,
								   const std::string & _data_block_path):
storage_driver(_storage_driver),
data_block_path(_data_block_path),
data_block(NULL) {
	
}

DataBlockFetcher::~DataBlockFetcher() {
	close();
}

int DataBlockFetcher::open() {
	int err = 0;
	//open the datablock file
	if((err = storage_driver->openBlock(data_block_path, block_flag::BlockFlagReadeable, &data_block))) {
		DBF_LERR_ << "Error openign datablock";
	}
	return err;
}

int DataBlockFetcher::close() {
	int err = 0;
	if(!data_block) return -1;
	//close the data block file
	if((err = storage_driver->closeBlock(data_block))) {
		DBF_LERR_ << "Error openign datablock";
	}
	data_block = NULL;
	return err;
}

//read data
int DataBlockFetcher::readData(uint64_t offset, uint32_t data_len, void **data_handler) {
	int err = 0;
	boost::unique_lock<boost::mutex> rlock(mutex_read_access);
	//got to offset
	if((err = storage_driver->seekg(data_block, offset, block_seek_base::BlockSeekBaseBegin))) {
		DBF_LERR_ << "Error going to offset " << err;
	} else {
		//we can read so we need to allcoate memory for reading
		uint32_t readed_byte;
		*data_handler = calloc(data_len, 1);
		if((err = storage_driver->read(data_block, *data_handler, data_len, readed_byte))) {
			DBF_LERR_ << "Error reading data " << err;
		} else {
			//we got data
		}
	}
	return 0;
}