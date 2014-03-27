//
//  PosixStorageDriver.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__PosixStorageDriver__
#define __CHAOSFramework__PosixStorageDriver__

#include "StorageDriver.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

namespace chaos {
	namespace data_service {
		namespace storage_system {
			
			//! storage driver setting
			struct PosixStorageDriverSetting :public StorageDriverSetting {
				std::string fsd_domain_path;
			};
			
			//! Posix implementation of the Storage Driver
			REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(PosixStorageDriver, StorageDriver) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(PosixStorageDriver)
				
				PosixStorageDriverSetting *setting;
				
				PosixStorageDriver(std::string);
			protected:
				int _openFile(std::string path);
				
			public:
				~PosixStorageDriver();
				
				//! init
				/*!
				 This implementation need a point to the PosixStorageDriverSetting struct
				 for his configuration;
				 */
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
				
				// open a block of a determinated type with
				int openBlock(chaos_vfs::BlockType type, std::string path, unsigned int flag, chaos_vfs::DataBlock **data_block);
				
				//! close the block of data
				int closeBlock(chaos_vfs::BlockType *data_block);

				//! return all block of data found on the path, acocrding to the type
				int listBlock(chaos_vfs::BlockType type, std::string path, boost::ptr_vector<chaos_vfs::DataBlock>& bloks_found);
				
				//! write an amount of data into a DataBlock
                int write(chaos_vfs::BlockType *data_block, void * data, uint32_t data_len);
				
				//! read an amount of data from a DataBlock
                int read(chaos_vfs::BlockType *data_block, uint64_t offset, void * * data, uint32_t *data_len);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__PosixStorageDriver__) */
