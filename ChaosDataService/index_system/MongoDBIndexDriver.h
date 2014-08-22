/*
 *	MongoDBIndexDriver.h
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
#ifndef __CHAOSFramework__MongoDBIndexDriver__
#define __CHAOSFramework__MongoDBIndexDriver__

#include "IndexDriver.h"
#include "MongoDBHAConnectionManager.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

namespace chaos {
	namespace data_service {
			namespace index_system {
				
				/*!
				 Collection for the storage for the vfs infroamtion of the virtual files across the domain
				 
				 //some index need to be setup on the mongo installation
				 usedb chaos_vfs
				 
				 //unique index for the vfs colletion
				 db.vfat.ensureIndex( { "vfs_path": 1, "vfs_domain":1 } , { unique: true } )
				 */
#define MONGO_DB_VFS_DB_NAME				"chaos"
#define MONGO_DB_VFS_DOMAINS_COLLECTION		"domains"
#define MONGO_DB_VFS_DOMAINS_URL_COLLECTION	"domains_url"
#define MONGO_DB_VFS_VFAT_COLLECTION		"vfat"
#define MONGO_DB_VFS_VBLOCK_COLLECTION		"datablock"
#define MONGO_DB_IDX_DATA_PACK_COLLECTION	"dp_idx"
				
#define MONGO_DB_COLLECTION_NAME(db,coll)	boost::str(boost::format("%1%.%2%") % db % coll)
				
				//domains field---------------------------------------------------
#define MONGO_DB_FIELD_DOMAIN_NAME			"domain_name"
#define MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE	"domain_unique_code"
#define MONGO_DB_FIELD_DOMAIN_URL			"domain_url"
#define MONGO_DB_FIELD_DOMAIN_HB			"hb"
				
				//file field------------------------------------------------------
#define MONGO_DB_FIELD_FILE_PRIMARY_KEY				"fpk"
#define MONGO_DB_FIELD_FILE_VFS_PATH				"vfs_path"
#define MONGO_DB_FIELD_FILE_VFS_DOMAIN				"vfs_domain"
				
				//data block field-------------------------------------------------
#define MONGO_DB_FIELD_DATA_BLOCK_STATE				"state"
#define MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS		"ct"
#define MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS	"vu"
#define MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE	"mbs"
#define MONGO_DB_FIELD_DATA_BLOCK_CUR_POSITION		"cur_pos"
#define MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH			"vfs_path"
#define MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN		"vfs_domain"
#define MONGO_DB_FIELD_DATA_BLOCK_HB				"hb"
				
				//db_idx field-------------------------------------------------
#define MONGO_DB_IDX_DATA_PACK_DID						"did"
#define MONGO_DB_IDX_DATA_PACK_ACQ_TS					"acq_ts"
#define MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_ID		"db_id"
#define MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET	"db_offset"
				
				//! Mongodb implementation for the index driver
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(MongoDBIndexDriver, IndexDriver) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBIndexDriver)
					MongoDBIndexDriver(std::string alias);
					std::string db_name;
				protected:
					MongoDBHAConnectionManager *ha_connection_pool;
					
				public:
					~MongoDBIndexDriver();
					
					//! init
					void init(void *init_data) throw (chaos::CException);
					
					//!deinit
					void deinit() throw (chaos::CException);
					//! Register a new domain
					int vfsAddDomain(vfs::VFSDomain domain);
					
					//! Give an heart beat for a domain
					int vfsDomainHeartBeat(vfs::VFSDomain domain);
					
					//! Register a new data block wrote on stage area
					int vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file,
										   chaos_vfs::DataBlock *data_block,
										   vfs::data_block_state::DataBlockState new_block_state = vfs::data_block_state::DataBlockStateNone);
					
					//! Set the state for a stage datablock
					int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block,
											   vfs::data_block_state::DataBlockState state);
					
					//! Set the state for a stage datablock
					int vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block,
											   vfs::data_block_state::DataBlockState cur_state,
											   vfs::data_block_state::DataBlockState new_state,
											   bool& success);
					
					//! Set the datablock current position
					virtual int vfsSetCurrentPositionOnDatablock(chaos_vfs::VFSFile *vfs_file,
																 chaos_vfs::DataBlock *data_block,
																 uint64_t offset);
					
					//! Set the datablock current position
					virtual int vfsSetHeartbeatOnDatablock(chaos_vfs::VFSFile *vfs_file,
														   chaos_vfs::DataBlock *data_block);
					
					//! Return the next available datablock created since timestamp
					int vfsFindSinceTimeDataBlock(chaos_vfs::VFSFile *vfs_file,
												  uint64_t timestamp,
												  bool direction,
												  vfs::data_block_state::DataBlockState state,
												  chaos_vfs::DataBlock **data_block);
					
					//! Heartbeat update stage block
					int vfsWorkHeartBeatOnDataBlock(chaos_vfs::VFSFile *vfs_file,
													chaos_vfs::DataBlock *data_block);
					
					//! Check if the vfs file exists
					int vfsFileExist(chaos_vfs::VFSFile *vfs_file,
									 bool& exists_flag);
					
					//! Create a file entry into the vfat
					int vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file);
					
					//! Return a list of vfs path of the file belong to a domain
					int vfsGetFilePathForDomain(std::string vfs_domain, std::string prefix_filter, std::vector<std::string>& result_vfs_file_path, int limit_to_size);
					
					//! add the default index for a unique instrument identification and a timestamp
					int idxAddDataPackIndex(const DataPackIndex& index);
					
					//! remove the index for a unique instrument identification and a timestamp
					virtual int idxDeleteDataPackIndex(const DataPackIndex& index);
				};
			}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBIndexDriver__) */
