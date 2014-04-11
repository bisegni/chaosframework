/*
 *	VFSFile.h
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
#ifndef __CHAOSFramework__VFSFile__
#define __CHAOSFramework__VFSFile__


#include "index_system/IndexDriver.h"
#include "storage_system/StorageDriver.h"


namespace chaos_data_index = chaos::data_service::index_system;
namespace chaos_data_storage = chaos::data_service::storage_system;

namespace chaos {
	namespace data_service {
		namespace vfs {
			
			class VFSManager;
			struct DataBlock;
			
			typedef struct VFSFileInfo {
				uint32_t		max_block_size;
				uint32_t		max_block_lifetime;
				std::string		identify_key;
				std::string		vfs_fpath;
				std::string		vfs_domain;
			} VFSFileInfo;
			
			//! VFS Logical file
			class VFSFile {
				friend class VFSManager;
				//!index driver pointer
				chaos_data_index::IndexDriver *index_driver_ptr;
				
				//!storage driver pointer
				chaos_data_storage::StorageDriver *storage_driver_ptr;

				std::string last_error;
				
				//! operational setting for the virtual file
				VFSFileInfo vfs_file_info;
				
				//! Current available data block where read or write
				DataBlock *current_data_block;
				
				uint32_t check_validity_counter;
				
				//default consturctor or destructor
				VFSFile(std::string vfs_fpath);
				~VFSFile();
				
				//! return new datablock where write into
				inline int getNewDataBlock(DataBlock **new_data_block_handler);
				
				//! release a datablock
				inline int releaseDataBlock(DataBlock *data_block_ptr);
				
				//! check if datablock is valid according to internal logic
				inline bool isDataBlockValid(DataBlock *new_data_blok_handler);
			public:
				//! Get the VFS information for file
				const VFSFileInfo *getVFSFileInfo() const;
				
				//! Check if the file exists
				bool exist();
				
				//! write data on the current data block
				int write(void *data, uint32_t data_len);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSFile__) */
