//
//  mongo_db_types.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 03/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_mongo_db_types_h
#define CHAOSFramework_mongo_db_types_h
namespace chaos {
	namespace data_service {
		namespace db_system {
			
			
			/*!
			 Collection for the storage for the vfs infroamtion of the virtual files across the domain
			 
			 //some index need to be setup on the mongo installation
			 usedb chaos_vfs
			 
			 //unique index for the vfs colletion
			 db.vfat.ensureIndex( { "vfs_path": 1, "vfs_domain":1 } , { unique: true } )
			 */
#define MONGO_DB_NAME						"chaos"
#define MONGO_DB_COLLECTION_VFS_DOMAINS		"domains"
#define MONGO_DB_COLLECTION_VFS_DOMAINS_URL	"domains_url"
#define MONGO_DB_COLLECTION_VFS_VFAT		"vfat"
#define MONGO_DB_COLLECTION_VFS_VBLOCK		"datablock"
#define MONGO_DB_COLLECTION_IDX_DATA_PACK	"dp_idx"
#define MONGO_DB_COLLECTION_SNAPSHOT		"snapshot"
#define MONGO_DB_COLLECTION_SNAPSHOT_DATA	"snapshot_data"
			
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
#define MONGO_DB_FIELD_DATA_BLOCK_STATE						"state"
#define MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS				"ct"
#define MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS			"vu"
#define MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE			"mbs"
#define MONGO_DB_FIELD_DATA_BLOCK_CURRENT_WORK_POSITION		"cur_work_pos"
#define MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH					"vfs_path"
#define MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN				"vfs_domain"
#define MONGO_DB_FIELD_DATA_BLOCK_HB						"hb"
			
			//db_idx field-------------------------------------------------
#define MONGO_DB_FIELD_IDX_DATA_PACK_DID						"did"
#define MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS					"acq_ts"
#define MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC			"acq_ts_numeric"
//#define MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_ID		"db_id"
#define MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN	"db_domain"
#define MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_PATH		"db_path"
#define MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET	"db_offset"
#define MONGO_DB_FIELD_IDX_DATA_PACK_SIZE						"dp_size"
#define MONGO_DB_FIELD_IDX_DATA_PACK_STATE					"dp_state"
			
			//snapshot field-------------------------------------------------
#define MONGO_DB_FIELD_SNAPSHOT_NAME		"snap_name"
#define MONGO_DB_FIELD_SNAPSHOT_TS			"snap_ts"

			//snapshot data-------------------------------------------------
#define MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME	"snap_name"
#define MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID	"producer_id"
//the key that contains the channel serialization is created at runtine with the semantic producer_key[_o,_i,_c,_s]
		}
	}
}


#endif
