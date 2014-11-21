/*
 *	DataBlockFetcher.h
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

#ifndef __CHAOSFramework__DataBlockFetcher__
#define __CHAOSFramework__DataBlockFetcher__


#include "../DataBlock.h"
#include "../storage_system/StorageDriver.h"

#include <boost/thread.hpp>
namespace chaos {
	namespace data_service {
		namespace vfs {
			namespace query {
				class DataBlockCache;
				//! class that manage the fecth of data pack on o a single datablock
				/*!
				 this class manage and optimize the multithreaded access to the single 
				 datablock.
				 */
				class DataBlockFetcher {
					friend class chaos::data_service::vfs::query::DataBlockCache;
					
					uint32_t use_count;
					
					//! storage driver for operation on datablock
					storage_system::StorageDriver *storage_driver;
					
					//! path pointing to the datablock
					std::string datablock_path;
					
					//! datablock structure
					vfs::DataBlock *datablock;
					
					//! data block mutex for mutltitrhead access
					boost::mutex mutex_read_access;
					
					DataBlockFetcher(storage_system::StorageDriver *_driver,
									 const std::string & _data_block_path);
					
					~DataBlockFetcher();
					
					//! open the datablock
					int open();
					
					//! open the datablock
					int close();
				public:
					//! read the data
					int readData(uint64_t offset, uint32_t data_len, void**data_handler);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DataBlockFetcher__) */
