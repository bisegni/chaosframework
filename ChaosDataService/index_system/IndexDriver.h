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

#include "../vfs/VFSTypes.h"

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
				
				
				//! Register a new domain
				/*!
				 Register a new domain adding an url. Different data service that
				 share the same domain wil register their own url
				 */
				virtual int vfsAddDomain(vfs::VFSDomain domain) = 0;
				
				//! Give an heart beat for a domain
				/*!
				 Give an heart beat for a domain
				 \param domain_name the name of the domain that the hb
				 */
				virtual int vfsDomainHeartBeat(vfs::VFSDomain domain) = 0;
				
				//! Register a new data block wrote on stage area
				/*!
				 Registration of a new datablock in stage area is achieved directly to the DataService process
				 after the block has been created.
				 \param vfs_file the vfs file for wich we need to create the new data block
				 \param data_block Newly created data block
				 */
				virtual int vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block,
											   vfs::data_block_state::DataBlockState new_block_state = vfs::data_block_state::DataBlockStateNone) = 0;
				
				//! Set the state for a datablock
				/*!
				 Set the current state for a datablock in the stage area
				 \param vfs_file owner of the datablock
				 \param data_block Data block for wich need to be changed the state
				 \param state new state to set
				 */
				virtual int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
												   chaos_vfs::DataBlock *data_block,
												   vfs::data_block_state::DataBlockState state) = 0;
				
				//! Set the state for a datablock
				/*!
				 Set the current state for a datablock if is current state
				 is equal to function param'cur_state'. The operation is done
				 only if the current state of the datablock is equal to the
				 given parameter, and the update is done atomically
				 \param vfs_file owner of the datablock
				 \param data_block Data block for wich need to be changed the state
				 \param cur_state current state of the block
				 \param new_state new state of the block
				 */
				virtual int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
												   chaos_vfs::DataBlock *data_block,
												   vfs::data_block_state::DataBlockState cur_state,
												   vfs::data_block_state::DataBlockState new_state,
												   bool& success) = 0;
				
				//! Return the next available datablock created since timestamp
				/*!
				 The rule on how select the Datablock is regulated by direction flag. It set the next or prev, starting from
				 the timestamp, datablock to select that match the state. The api is atomic
				 \param vfs_file virtual file at wich the datablock belowng
				 \param timestamp the timestamp form wich search
				 \param directio true -> enxt or false -> prev
				 \param state the state for the selection of the datablock
				 \param data_block the returned, if found, datablock
				 \return the error code
				 */
				virtual int vfsFindSinceTimeDataBlock(chaos_vfs::VFSFile *vfs_file,
													  uint64_t timestamp,
													  bool direction,
													  vfs::data_block_state::DataBlockState state,
													  chaos_vfs::DataBlock **data_block) = 0;
				
				//! Heartbeat update stage block
				/*!
				 Registration of a new datablock in stage area is achieved directly to the DataService process
				 after the block has been created.
				 */
				virtual int vfsWorkHeartBeatOnDataBlock(chaos_vfs::VFSFile *vfs_file,
														chaos_vfs::DataBlock *data_block) = 0;
				
				//! Check if the vfs file exists
				/*!
				 Check on vfs index db if the file exists
				 */
				virtual int vfsFileExist(chaos_vfs::VFSFile *vfs_file,
										 bool& exists_flag) = 0;
				
				//! Create a file entry into the vfat
				/*!
				 allocate an entry on the database for vfile
				 */
				virtual int vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file) = 0;
				
				//! Return a list of vfs path of the file belong to a domain
				/*!
				 The returned vfs file paths are filtered using the prefix, so all
				 path are of the form PREFIX_other_character
				 \param vfs_domain the domain that we wan't to selct
				 \param prefix_filter the prefix of the path used for filter the result
				 \param result_vfs_file_path array contains the found vfs paths.
				 */
				virtual int vfsGetFilePathForDomain(std::string vfs_domain, std::string prefix_filter, std::vector<std::string>& result_vfs_file_path, int limit_to_size = 100) = 0;
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__IndexDriver__) */
