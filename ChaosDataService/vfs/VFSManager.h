/*
 *	VFSManager.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__VFSManager__
#define __CHAOSFramework__VFSManager__

#include "VFSTypes.h"
#include "VFSFile.h"
#include "VFSStageWriteableFile.h"
#include "VFSDataWriteableFile.h"
#include "VFSStageReadableFile.h"
#include "VFSQuery.h"
#include "../db_system/DBDriver.h"
#include "storage_system/storage_system.h"

#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/TemplatedKeyObjectContainer.h>
#include <chaos/common/utility/InizializableService.h>


namespace chaos {
	namespace data_service {
		
		namespace vfs {
			
			
#define VFSManager_MAX_BLOCK_SIZE		1024*1024*10		// 10 megabyte
#define VFSManager_MAX_BLOCK_LIFETIME	1000*60*5			// 5 minutes
			
			//! VFS file manager setting
			typedef struct VFSManagerSetting {
				//! identify the max size for a logical file block
				uint32_t		max_block_size;
				
				//! identify the mac lifetime for a logical file block
				uint32_t		max_block_lifetime;
				
				//! current storage driver implementaiton to use
				std::string		storage_driver_impl;
				
				//! the instance of the storage driver for this manager
				storage_system::StorageDriverSetting storage_driver_setting;
			}VFSManagerSetting;
			
			typedef std::map<std::string,VFSFile *> FileInstanceMap;
			typedef std::map<std::string,VFSFile *>::iterator FileInstanceMapIterator;
			
			typedef struct VFSFilesForPath {
				boost::mutex	_mutex;
				uint32_t		instance_counter;
				FileInstanceMap	map_logical_files;
			} VFSFilesForPath;
			
			typedef common::utility::TemplatedKeyObjectContainer<std::string, VFSFilesForPath*> VFSManagerKeyObjectContainer;
			
			class VFSManager :
			public common::utility::InizializableService,
			public chaos::common::async_central::TimerHandler,
			protected VFSManagerKeyObjectContainer {
				//! point to user allcoated configuration structure, the instance
				//! will be deallocated at thedestruction of this class
				VFSManagerSetting *setting;
				
				//!index driver pointer
				db_system::DBDriver *db_driver_ptr;
				
				//!storage driver pointer
				storage_system::StorageDriver *storage_driver_ptr;
				
				void timeout();
				//void giveDomainHeartbeat(const boost::system::error_code& e);
			protected:
				void freeObject(std::string key, VFSFilesForPath *element);
			public:
				VFSManager(db_system::DBDriver *_db_driver_ptr);
				~VFSManager();
				
				//! Need to be forwarded a point to a structure VFSManagerSetting
				void init(void * init_data) throw (CException);
				
				void deinit() throw (CException);
				
				int getFile(std::string area, std::string vfs_path, VFSFile **l_file, VFSFileOpenMode open_mode);
				
				int getWriteableStageFile(std::string stage_vfs_relative_path, VFSStageWriteableFile **wsf_file);
				
				int getReadableStageFile(std::string stage_vfs_relative_path, VFSStageReadableFile **rsf_file);
				
				int getWriteableDataFile(std::string data_vfs_relative_path, VFSDataWriteableFile **wdf_file);
				
				int getVFSQuery(const chaos::data_service::db_system::DataPackIndexQuery& _query, VFSQuery **vfs_query);
				
				int releaseFile(VFSFile *l_file);
				
				int createDirectory(std::string vfs_path, bool all_path = false);
				
				int deleteDirectory(std::string vfs_path, bool all_path = false);
				
				int getAllStageFileVFSPath(std::vector<std::string>& stage_file_vfs_paths, int limit_to_size = 100);
				
				//! return the next available indexable stage file
				int getNextIndexableStageFileVFSPath(std::string& indexable_stage_file_vfs_path);
				
				//! change the state to all timeouted chunk
				/*!
					Find in the area selected the datablock that are in timeout.
					A datablock is considered in timeout when his heartbeat filed
					is aged of timeout_delay number of second. The considered datablock are identified by
					timeout_state and are changed to new_state.
				 \param stage_data is the ared in wich is effected the search
				 \param timeout_delay is the delay for the heartbeat that is to be considered as timeouted
				 \param timeout_state is the state of the datablock that are included in the search
				 \param new_state is the new state to set to all datablock that are in timeout
				 */
				int changeStateToTimeoutedDatablock(bool stage_data,
													uint32_t timeout_delay,
													data_block_state::DataBlockState timeout_state,
													data_block_state::DataBlockState new_state);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSManager__) */
