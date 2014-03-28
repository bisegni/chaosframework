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

#include "../vfs/vfs.h"

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>
#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos_vfs = chaos::data_service::vfs;

namespace chaos {
	namespace data_service {
		namespace storage_system {
			
			//! storage driver setting
			struct StorageDriverSetting {
				std::string fs_domain_name;
			};

#define SD_STAGE_AREA_ALIAS "stage"
#define SD_DATA_AREA_ALIAS "data"
			
			//! Abstraction of the vfs driver
			/*!
			 This class represent the abstraction of the
			 work to do on chaos VFS
			 */
			class StorageDriver : public chaos::NamedService, public chaos::utility::InizializableService {

			protected:
				//! absolute path to the stage area
				std::string path_stage_area;
				
				//! absolute path to the data area
				std::string path_data_area;
				
				//! Driver sepcific configration
				StorageDriverSetting *setting;
				
				//! Protected constructor
				StorageDriver(std::string alias);
				
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
				
				//! open a block of a determinated type with
				/*!
				 The returned block is a struct that desbribe a data block
				 can we can write or read within.
				 \param type the type of the block we want to open
				 \param path the path relative to the type of the block
				 \flag the flag for determinate the mode if the open operation it can be a concatenation of chaos::data_service::chaos_vfs::block_flag::DataBlockFlag
				 \
				 */
				virtual int openBlock(chaos_vfs::block_type::BlockType type, std::string path, unsigned int flags, chaos_vfs::DataBlock **data_block) = 0;
				
				//! close the block of data
				virtual int closeBlock(chaos_vfs::block_type::BlockType *data_block) = 0;
				
				//! return all block of data found on the path, acocrding to the type
				virtual int listBlock(chaos_vfs::block_type::BlockType type, std::string path, boost::ptr_vector<chaos_vfs::DataBlock>& bloks_found) = 0;

				//! write an amount of data into a DataBlock
                virtual int write(chaos_vfs::block_type::BlockType *data_block, void * data, uint32_t data_len) = 0;
				
				//! read an amount of data from a DataBlock
                virtual int read(chaos_vfs::block_type::BlockType *data_block, uint64_t offset, void * * data, uint32_t *data_len) = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__StorageDriver__) */
