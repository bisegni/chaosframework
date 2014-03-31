/*
 *	VFSLogicalFile.h
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
#ifndef __CHAOSFramework__VFSLogicalFile__
#define __CHAOSFramework__VFSLogicalFile__

#include "../index_system/IndexDriver.h"
#include "../storage_system/StorageDriver.h"


namespace chaos_data_index = chaos::data_service::index_system;
namespace chaos_data_storage = chaos::data_service::storage_system;

namespace chaos {
	namespace data_service {
		namespace vfs {
			
			class VFSFileManager;
			
			typedef struct VFSLogicalFileInfo {
				uint32_t		max_block_size;
				uint32_t		max_block_lifetime;
				std::string		identify_key;
				std::string		vfs_fpath;
			} VFSLogicalFileInfo;
			
			//! VFS Logical file
			class VFSLogicalFile {
				friend class VFSFileManager;
				
				//!index driver pointer
				chaos_data_index::IndexDriver *index_driver_ptr;
				
				//!storage driver pointer
				chaos_data_storage::StorageDriver *storage_driver_ptr;

				std::string last_error;
				VFSLogicalFileInfo vfs_file_info;
				
				VFSLogicalFile(std::string vfs_fpath);
				~VFSLogicalFile();
			public:
				
				bool exist();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSLogicalFile__) */
