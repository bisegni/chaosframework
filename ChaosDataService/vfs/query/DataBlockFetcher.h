//
//  DataBlockFetcher.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 09/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DataBlockFetcher__
#define __CHAOSFramework__DataBlockFetcher__


#include "../DataBlock.h"
#include "../storage_system/StorageDriver.h"

#include <boost/thread.hpp>
namespace chaos {
	namespace data_service {
		namespace vfs {
			class VFSQuery;
			
			namespace query {
				
				//! class that manage the fecth of data pack on o a single datablock
				/*!
				 this class manage and optimize the multithreaded access to the single 
				 datablock.
				 */
				class DataBlockFetcher {
					friend class chaos::data_service::vfs::VFSQuery;
					
					//! storage driver for operation on datablock
					storage_system::StorageDriver *storage_driver;
					
					//! path pointing to the datablock
					std::string data_block_path;
					
					//! datablock structure
					vfs::DataBlock *data_block;
					
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
