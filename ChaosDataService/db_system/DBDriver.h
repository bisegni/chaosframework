/*
 *	DBDriver.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__DBDriver__
#define __CHAOSFramework__DBDriver__

#include <map>
#include <string>
#include <vector>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

#include "db_system_types.h"
#include "DBIndexCursor.h"

namespace chaos {
	namespace data_service {
		
		namespace vfs {
			class VFSFile;
			struct DataBlock;
		}
		namespace chaos_vfs = chaos::data_service::vfs;
		
		namespace db_system {
			/*!
			 Base class for all driver that will manage the work on index database.
			 The base role of the index driver is to help the fast storage
			 of into stage area, coordinate the worker that migrate data between stage and data area
			 of the !CHAOS virtual filesystem. At the end it will help to achive the execution
			 of query on memoryzed data
			 */
			class DBDriver : public NamedService , public chaos::utility::InizializableService {
			protected:
				//protected constructor
				DBDriver(std::string alias);
			protected:
				DBDriverSetting *setting;
				
				chaos_vfs::DataBlock *getDataBlockFromFileLocation(const vfs::FileLocationPointer& data_block);
				uint64_t getDataBlockOffsetFromFileLocation(const vfs::FileLocationPointer& data_block);
			public:
				
				//! public destructor
				virtual ~DBDriver();
				
				//! init
				/*!
				 Need a point to a structure DBDriverSetting for the setting
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
				
				//! Delete a virtual file datablock
				/*!
				 Delete a datablock associated to a virtual file
				 */
				virtual int vfsDeleteDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block) = 0;
				
				//! Set the state for a datablock
				/*!
				 Set the current state for a datablock in the stage area
				 \param vfs_file owner of the datablock
				 \param data_block Data block for wich need to be changed the state
				 \param state new state to set (chaos::data_service::vfs::data_block_state::DataBlockState)
				 */
				virtual int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
												   chaos_vfs::DataBlock *data_block,
												   int state) = 0;
				
				//! Set the state for a datablock
				/*!
				 Set the current state for a datablock if is current state
				 is equal to function param'cur_state'. The operation is done
				 only if the current state of the datablock is equal to the
				 given parameter, and the update is done atomically
				 \param vfs_file owner of the datablock
				 \param data_block Data block for wich need to be changed the state
				 \param cur_state current state of the block (chaos::data_service::vfs::data_block_state::DataBlockState)
				 \param new_state new state of the block (chaos::data_service::vfs::data_block_state::DataBlockState)
				 */
				virtual int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
												   chaos_vfs::DataBlock *data_block,
												   int cur_state,
												   int new_state,
												   bool& success) = 0;
				
				//! Set the datablock current position
				virtual int vfsSetHeartbeatOnDatablock(chaos_vfs::VFSFile *vfs_file,
													   chaos_vfs::DataBlock *data_block,
													   uint64_t timestamp = 0) = 0;
				
				
				//! update the current datablock size
				virtual int vfsUpdateDatablockCurrentWorkPosition(chaos_vfs::VFSFile *vfs_file,
																  chaos_vfs::DataBlock *data_block) = 0;
				
				//! Return the next available datablock created since timestamp
				/*!
				 The rule on how select the Datablock is regulated by direction flag. It set the next or prev, starting from
				 the timestamp, datablock to select that match the state. The api is atomic
				 \param vfs_file virtual file at wich the datablock belowng
				 \param timestamp the timestamp form wich search
				 \param direction true -> enxt or false -> prev
				 \param state the state for the selection of the datablock (chaos::data_service::vfs::data_block_state::DataBlockState)
				 \param data_block the returned, if found, datablock
				 \return the error code
				 */
				virtual int vfsFindSinceTimeDataBlock(chaos_vfs::VFSFile *vfs_file,
													  uint64_t timestamp,
													  bool direction,
													  int state,
													  chaos_vfs::DataBlock **data_block) = 0;
				
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
				virtual int vfsGetFilePathForDomain(const std::string& vfs_domain,
													const std::string& prefix_filter,
													std::vector<std::string>& result_vfs_file_path,
													int limit_to_size = 100) = 0;
				
				//! add the default index for a unique instrument identification and a timestamp
				/*!
				 whitin !CHAOS every data pack has, by default, an unique identification and a timestamp that represet
				 the time when the data into packet have been colelcted. This api write the default index on database
				 that permit to find the data pack whhitin the destination virtula file.
				 \param index the index to be stored for a new datapack
				 */
				virtual int idxAddDataPackIndex(const DataPackIndex& index) = 0;
				
				//! remove the index for a unique instrument identification and a timestamp
				virtual int idxDeleteDataPackIndex(const DataPackIndex& index) = 0;
				
				//! set the state on all datapack index for an datablock
				virtual int idxSetDataPackIndexStateByDataBlock(const std::string& vfs_datablock_domain,
																const std::string& vfs_datablock_path,
																DataPackIndexQueryState dp_index_state) = 0;
				
				//! perform a search on data pack indexes
				/*!
				 start a query on index database  and return the cursor for paging the result
				 \param _query the query
				 \param index_cursor paged cursor for retrieve the result
				 */
				virtual int idxStartSearchDataPack(const chaos::data_service::db_system::DataPackIndexQuery& _query, DBIndexCursor **index_cursor) = 0;
				
				//! Create a new snapshot
				/*!
				 Create a new snapshot with the name
				 \param snapshot_name the name of the new snapshot
				 \param the new job identification id, subseguent operation on snapshot need to be done using this code, otherwise
				 they will fails.
				 */
				virtual int snapshotCreateNewWithName(const std::string& snapshot_name,
													  std::string& working_job_unique_id) = 0;
				
				//! Add an element to a named snapshot
				/*!
				 add an element to the snapshot
				 \param working_job_unique_id the identification of the job
				 \param snapshot_name the name of the snapshot where put the element
				 \param producer_unique_key the unique key of the producer
				 \param dataset_type the type of the dataset, refer to @DataPackCommonKey::DPCK_DATASET_TYPE field of the dataset
				 \param data the serialized data of the dataset
				 \param data_len the length of the serialized data
				 */
				virtual int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
														 const std::string& snapshot_name,
														 const std::string& producer_unique_key,
														 const std::string& dataset_type,
														 void* data,
														 uint32_t data_len) = 0;
				
				//! Increment or decrement the number of the job that are working on
				/*!
				 this function permit to manage the number of job that are working on the snapshot
				 \param working_job_unique_id the code associated to the job
				 \param snapshot_name the name of the snapshot to modify
				 \param add if true it add +1 if false add -1
				 */
				virtual int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
														const std::string& snapshot_name,
														bool add) = 0;
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DBDriver__) */
