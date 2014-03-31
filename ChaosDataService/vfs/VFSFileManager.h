/*
 *	VFSFileManager.h
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
#ifndef __CHAOSFramework__VFSFileManager__
#define __CHAOSFramework__VFSFileManager__

#include "VFSLogicalFile.h"

#include "../index_system/IndexDriver.h"
#include "../storage_system/StorageDriver.h"

#include <chaos/common/utility/TemplatedKeyObjectContainer.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos_data_index = chaos::data_service::index_system;
namespace chaos_data_storage = chaos::data_service::storage_system;

namespace chaos {
	namespace data_service {
		
		namespace vfs {
			
			//! VFS file manager setting
			typedef struct VFSFileManagerSetting {
				//! identify the max size for a logical file block
				uint32_t		max_block_size;
				
				//! identify the mac lifetime for a logical file block
				uint32_t		max_block_lifetime;
				
				//! current index driver implementaiton to use
				std::string index_driver_impl;
				
				//! the instance of the index driver for this manager
				chaos_data_index::IndexDriverSetting *index_driver_setting;
				
				//! current storage driver implementaiton to use
				std::string storage_driver_impl;
				
				//! the instance of the storage driver for this manager
				chaos_data_storage::StorageDriverSetting *storage_driver_setting;
			}VFSFileManagerSetting;
			
			typedef std::map<std::string,VFSLogicalFile *> FileInstanceMap;
			typedef std::map<std::string,VFSLogicalFile *>::iterator FileInstanceMapIterator;
			
			typedef struct VFSFilesForPath {
				boost::mutex	_mutex;
				uint32_t		instance_counter;
				FileInstanceMap	map_logical_files;
			} VFSFilesForPath;
			
			typedef chaos::utility::TemplatedKeyObjectContainer<std::string, VFSFilesForPath*> VFSFileManagerKeyObjectContainer;
			
			class VFSFileManager :
			public chaos::utility::InizializableService,
			protected VFSFileManagerKeyObjectContainer {
				
				//! point to user allcoated configuration structure, the instance
				//! will be deallocated at thedestruction of this class
				VFSFileManagerSetting *setting;
				
				//!index driver pointer
				chaos_data_index::IndexDriver *index_driver_ptr;
				
				//!storage driver pointer
				chaos_data_storage::StorageDriver *storage_driver_ptr;
				
			protected:
				void freeObject(std::string key, VFSFilesForPath *element);
				
			public:
				VFSFileManager();
				~VFSFileManager();
				
				//! Need to be forwarded a point to a structure VFSFileManagerSetting
				void init(void * init_data) throw (CException);
				
				void deinit() throw (CException);
				
				int getLogicalFile(std::string vfs_path, VFSLogicalFile **l_file);
				
				int relaseLogicalFile(VFSLogicalFile *l_file);
			};

		}
	}
}

#endif /* defined(__CHAOSFramework__VFSFileManager__) */
