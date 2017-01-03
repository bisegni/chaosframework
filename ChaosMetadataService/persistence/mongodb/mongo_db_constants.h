//
//  mongo_db_constants.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 30/01/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <chaos/common/chaos_constants.h>

#include <boost/format.hpp>

#ifndef CHAOSFramework_mongo_db_constants_h
#define CHAOSFramework_mongo_db_constants_h

static const char * const MONGO_DB_NAME						= "chaos";
static const char * const MONGO_DB_COLLECTION_VFS_DOMAINS	= 	"domains";
static const char * const MONGO_DB_COLLECTION_VFS_DOMAINS_URL	= "domains_url";
static const char * const MONGO_DB_COLLECTION_VFS_VFAT		= "vfat";
static const char * const MONGO_DB_COLLECTION_VFS_VBLOCK		= "datablock";
static const char * const MONGO_DB_COLLECTION_IDX_DATA_PACK	= "dp_idx";
static const char * const MONGO_DB_COLLECTION_SNAPSHOT		= "snapshot";
static const char * const MONGO_DB_COLLECTION_SNAPSHOT_DATA	= "snapshot_data";
//----------------------------------collection naming-------------------------------------------
static const char * const MONGODB_COLLECTION_NODES                      = "mds_nodes";
static const char * const MONGODB_COLLECTION_NODES_COMMAND              = "mds_nodes_command";
static const char * const MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE     = "mds_nodes_command_template";
static const char * const MONGODB_COLLECTION_SEQUENCES                  = "mds_sequences";
static const char * const MONGODB_COLLECTION_VARIABLES                  = "mds_variables";
//-------snapshot data------
static const char * const MONGODB_COLLECTION_SNAPSHOT                   = "snapshot";
static const char * const MONGODB_COLLECTION_SNAPSHOT_DATA              = "snapshot_data";

//-------tree group---------
static const char * const MONGODB_COLLECTION_TREE_GROUP                 = "mds_tree_group";

//---------logging----------
static const char * const MONGODB_COLLECTION_LOGGING                    = "mds_logging";

//---------script----------
static const char * const MONGODB_COLLECTION_SCRIPT                     = "mds_script";

//----------control unit ageing filed---------
static const char * const MONGODB_COLLECTION_NODES_AGEING_INFO              = "ageing_management";
static const char * const MONGODB_COLLECTION_NODES_AGEING_LAST_CHECK_DATA   = "last_ageing_check_time";
static const char * const MONGODB_COLLECTION_NODES_PROCESSING_AGEING        = "processing_ageing";
static const char * const MONGODB_COLLECTION_NODES_PERFORMED_AGEING        = "last_performed_ageing";

//domains field---------------------------------------------------
static const char * const MONGO_DB_FIELD_DOMAIN_NAME			= "domain_name";
static const char * const MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE	= "domain_unique_code";
static const char * const MONGO_DB_FIELD_DOMAIN_URL			= "domain_url";
static const char * const MONGO_DB_FIELD_DOMAIN_HB			= "hb";

//file field------------------------------------------------------
static const char * const MONGO_DB_FIELD_FILE_PRIMARY_KEY				= "fpk";
static const char * const MONGO_DB_FIELD_FILE_VFS_PATH				= "vfs_path";
static const char * const MONGO_DB_FIELD_FILE_VFS_DOMAIN				= "vfs_domain";

//data block field-------------------------------------------------
static const char * const MONGO_DB_FIELD_DATA_BLOCK_STATE						= "state";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS				= "ct";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS			= "vu";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE			= "mbs";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_CURRENT_WORK_POSITION		= "cur_work_pos";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH				= 	"vfs_path";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN				= "vfs_domain";
static const char * const MONGO_DB_FIELD_DATA_BLOCK_HB						= "hb";

//db_idx field-------------------------------------------------
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_DID					= "did";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_TYPE					= "type";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS					= "acq_ts";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC			= "acq_ts_numeric";
//static const char * const MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_ID		"db_id"
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN	= "db_domain";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_PATH		= "db_path";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET	= "db_offset";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_SIZE						= "dp_size";
static const char * const MONGO_DB_FIELD_IDX_DATA_PACK_STATE					= "dp_state";

//snapshot field-------------------------------------------------
static const char * const MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE				= "check_code";
static const char * const MONGO_DB_FIELD_SNAPSHOT_NAME					= "snap_name";
static const char * const MONGO_DB_FIELD_SNAPSHOT_TS						= "snap_ts";
static const char * const MONGO_DB_FIELD_SNAPSHOT_TS_DATE                = "snap_ts_date";
static const char * const MONGO_DB_FIELD_SNAPSHOT_JOB_CONCURRENCY			= "job_concurency";


//snapshot data-------------------------------------------------
static const char * const MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME	= "snap_name";
static const char * const MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID	= "producer_id";
//the key that contains the channel serialization is created at runtine with the semantic producer_key[_o,_i,_c,_s]




#endif
