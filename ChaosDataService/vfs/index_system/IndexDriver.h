/*
 *	IndexDriver.h
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

#ifndef __CHAOSFramework__IndexDriver__
#define __CHAOSFramework__IndexDriver__

#include <string>
#include <vector>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>


namespace chaos {
	namespace data_service {
		
		namespace vfs {
			class VFSFile;
			struct DataBlock;
		}
		
		namespace chaos_vfs = chaos::data_service::vfs;

		namespace index_system {
			
			typedef std::vector<std::string>			IndexDriverServerList;
			typedef std::vector<std::string>::iterator	IndexDriverServerListIterator;
			
			//! index driver setting
			typedef struct IndexDriverSetting {
				//! vector for all index endpoint url (usualy database url, more is the driver permit the scalability of the service)
				IndexDriverServerList servers;
				
				//! kv param for the implementations of the driver
				std::map<string,string> key_value_custom_param;
			} IndexDriverSetting;
			
			//! define a type of work that can occour on stage datablock
			typedef enum DataBlockState {
				//! block is not used for processing or acquisition
				DataBlockStateNone			= 0,
				
				//! block is acquiring new data
				DataBlockStateAquiringData	= 1,
				
				//! block is reading for processing
				DataBlockStateProcessing	= 2,
				
				//! block as been processed
				DataBlockStateProcessed		= 4,
				
				//! block as been locked for processing
				DataBlockStateLocked		= 8,
				
				//! block has been unlocked
				DataBlockStateUnlocked		= 12
			} DataBlockState;
			
			/*!
			 Base class for all driver that will manage the work on index database.
			 The base role of the index driver is to help the fast storage
			 of into stage area, coordinate the worker that migrate data between stage and data area
			 of the !CHAOS virtual filesystem. At the end it will help to achive the execution 
			 of query on memoryzed data
			 */
			class IndexDriver : public NamedService , public chaos::utility::InizializableService {
			protected:
				//protected constructor
				IndexDriver(std::string alias);
			protected:
				IndexDriverSetting *setting;
            public:
				
				//! public destructor
				virtual ~IndexDriver();
				
				//! init
				/*!
				 Need a point to a structure IndexDriverSetting for the setting
				*/
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
				
				//! Register a new data block wrote on stage area
				/*!
					Registration of a new datablock in stage area is achieved directly to the DataService process
					after the block has been created.
					\param vfs_file the vfs file for wich we need to create the new data block
					\param data_block Newly created data block
				 */
				virtual int vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block, DataBlockState new_block_state = DataBlockStateNone) = 0;
				
				//! Set the state for a stage datablock
				/*!
				 Set the current state for a datablock in the stage area
				 \param data_block Data block for wich need to be changed the state
				 */
				virtual int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,chaos_vfs::DataBlock *data_block, DataBlockState state) = 0;
				
				//! Heartbeat update stage block
				/*!
				 Registration of a new datablock in stage area is achieved directly to the DataService process
				 after the block has been created.
				 */
				virtual int vfsWorkHeartBeatOnDataBlock(chaos_vfs::VFSFile *vfs_file,chaos_vfs::DataBlock *data_block) = 0;

				//! Check if the vfs file exists
				/*!
				 Check on vfs index db if the file exists
				 */
				virtual int vfsFileExist(chaos_vfs::VFSFile *vfs_file, bool& exists_flag) = 0;
				
				//! Create a file entry into the vfat
				/*!
				 allocate an entry on the database for vfile
				 */
				virtual int vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file) = 0;
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__IndexDriver__) */
