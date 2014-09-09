//
//  DataBlockFetcher.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 09/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "DataBlockFetcher.h"

using namespace chaos::data_service::vfs::query;

DataBlockFetcher::DataBlockFetcher(storage_system::StorageDriver *_storage_driver,
								   const std::string & _data_block_path):
storage_driver(_storage_driver),
data_block_path(_data_block_path),
data_block(NULL) {
	
}

DataBlockFetcher::~DataBlockFetcher() {
	
}

int DataBlockFetcher::readData(uint64_t offset, uint32_t data_len, void**data_handler) {
	return 0;
}