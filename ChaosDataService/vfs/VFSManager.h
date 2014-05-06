/*
 *	VFSManager.h
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
#ifndef __CHAOSFramework__VFSManager__
#define __CHAOSFramework__VFSManager__

#include "VFSTypes.h"
#include "VFSFile.h"
#include "index_system/IndexDriver.h"
#include "storage_system/StorageDriver.h"

#include <chaos/common/utility/TemplatedKeyObjectContainer.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
namespace chaos_data_index = chaos::data_service::index_system;
namespace chaos_data_storage = chaos::data_service::storage_system;


namespace chaos {
	namespace data_service {
		
		namespace vfs {
			
#define VFS_STAGE_AREA	"stage/"
#define VFS_DATA_AREA	"data/"
			
			//! VFS file manager setting
			typedef struct VFSManagerSetting {
				//! identify the max size for a logical file block
				uint32_t		max_block_size;
				
				//! identify the mac lifetime for a logical file block
				uint32_t		max_block_lifetime;
				
				//! current index driver implementaiton to use
				std::string index_driver_impl;
				
				//! the instance of the index driver for this manager
				chaos_data_index::IndexDriverSetting index_driver_setting;
				
				//! current storage driver implementaiton to use
				std::string storage_driver_impl;
				
				//! the instance of the storage driver for this manager
				chaos_data_storage::StorageDriverSetting storage_driver_setting;
			}VFSManagerSetting;
			
			typedef std::map<std::string,VFSFile *> FileInstanceMap;
			typedef std::map<std::string,VFSFile *>::iterator FileInstanceMapIterator;
			
			typedef struct VFSFilesForPath {
				boost::mutex	_mutex;
				uint32_t		instance_counter;
				FileInstanceMap	map_logical_files;
			} VFSFilesForPath;
			
			typedef chaos::utility::TemplatedKeyObjectContainer<std::string, VFSFilesForPath*> VFSManagerKeyObjectContainer;
			
			class VFSManager :
			public chaos::utility::InizializableService,
			protected VFSManagerKeyObjectContainer {
				boost::asio::io_service io;
				boost::asio::deadline_timer timer;
				boost::scoped_ptr<boost::thread> thread_hb;
				//! point to user allcoated configuration structure, the instance
				//! will be deallocated at thedestruction of this class
				VFSManagerSetting *setting;
				
				//!index driver pointer
				chaos_data_index::IndexDriver *index_driver_ptr;
				
				//!storage driver pointer
				chaos_data_storage::StorageDriver *storage_driver_ptr;
				
				void giveDomainHeartbeat(const boost::system::error_code& e);
			protected:
				void freeObject(std::string key, VFSFilesForPath *element);
				
			public:
				VFSManager();
				~VFSManager();
				
				//! Need to be forwarded a point to a structure VFSManagerSetting
				void init(void * init_data) throw (CException);
				
				void deinit() throw (CException);
				
				int getFile(std::string vfs_path, VFSFile **l_file);
				
				int releaseFile(VFSFile *l_file);
				
				int createDirectory(std::string vfs_path, bool all_path = false);
				
				int deleteDirectory(std::string vfs_path, bool all_path = false);
			};

		}
	}
}

#endif /* defined(__CHAOSFramework__VFSManager__) */
