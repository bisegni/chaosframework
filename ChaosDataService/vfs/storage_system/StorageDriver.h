/*
 *	StorageDriver.h
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
#ifndef __CHAOSFramework__StorageDriver__
#define __CHAOSFramework__StorageDriver__

#include <string>
#include <vector>
#include "../DataBlock.h"

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos_vfs = chaos::data_service::vfs;

namespace chaos {
	namespace data_service {
		namespace storage_system {
			
			//! storage driver setting
			typedef struct StorageDriverSetting {
				//!driver domain name
				std::string fs_domain_name;
				
				//! kv param for the implementations of the driver
				std::map<string,string> key_value_custom_param;
			} StorageDriverSetting;


			
			//! Abstraction of the vfs driver
			/*!
			 This class represent the abstraction of the
			 work to do on chaos VFS
			 */
			class StorageDriver : public chaos::NamedService, public chaos::utility::InizializableService {

			protected:				
				
				//! Driver sepcific configration
				StorageDriverSetting *setting;
				
				//! Protected constructor
				StorageDriver(std::string alias);
				
			protected:
				chaos_vfs::DataBlock *getNewDataBlock(std::string path);
				
				void disposeDataBlock(chaos_vfs::DataBlock *);
				
				//! domain initialization
				/*!
				 Subclass need to implement his domain creation strategy
				 to permit that the same drivers on same storage share the same
				 domain
				 */
				virtual void initDomain() throw (chaos::CException) = 0;
            public:
				
				//! public destructor
				virtual ~StorageDriver();
				
				//! init
				/*!
				 This implementation need a point to the StorageDriverSetting struct
				 for his configuration;
				 */
				void init(void *init_data) throw (chaos::CException);
				
				//! deinit
				void deinit() throw (chaos::CException);

				//! return the storage domain for this driver
				const char * getStorageDomain() const;
				
				//! Open a block
				/*!
				 Open a block filled eith all information to be opened
				 \param type the type of the block we want to open
				 \return error code
				 */
				virtual int openBlock(chaos_vfs::DataBlock *data_block, unsigned int flags) = 0;

				//! open a block of a determinated type
				/*!
				 The returned block is a struct that desbribe a data block
				 can we can write or read within.
				 \param type the type of the block we want to open
				 \param path the path relative to the type of the block
				 \flag the flag for determinate the mode if the open operation it can be a concatenation of chaos::data_service::chaos_vfs::block_flag::DataBlockFlag
				 \
				 */
				virtual int openBlock(std::string vfs_path, unsigned int flags, chaos_vfs::DataBlock **data_block) = 0;
				
				//! close the block of data
				virtual int closeBlock(chaos_vfs::DataBlock *data_block) = 0;
				
				//! return the block current size
				virtual int getBlockSize(chaos_vfs::DataBlock *data_block) = 0;
				
				//! return all block of data found on the path, acocrding to the type
				virtual int listBlock(std::string vfs_path, std::vector<chaos_vfs::DataBlock*>& bloks_found) = 0;

				//! write an amount of data into a DataBlock
                virtual int write(chaos_vfs::DataBlock *data_block, void * data, uint32_t data_len) = 0;
				
				//! read an amount of data from a DataBlock
                virtual int read(chaos_vfs::DataBlock *data_block, uint64_t offset, void * * data, uint32_t& data_len) = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__StorageDriver__) */
