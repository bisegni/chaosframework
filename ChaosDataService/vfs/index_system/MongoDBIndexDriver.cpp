//
//  MongoDBIndexDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MongoDBIndexDriver.h"

#include "../vfs.h"

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::data_service::vfs;
using namespace chaos::data_service::vfs::index_system;

namespace chaos_data = chaos::common::data;

#define MongoDBIndexDriver_LOG_HEAD "[MongoDBIndexDriver] - "
#define MDBID_LAPP_ LAPP_ << MongoDBIndexDriver_LOG_HEAD
#define MDBID_LDBG_ LDBG_ << MongoDBIndexDriver_LOG_HEAD << __FUNCTION__ << " - "
#define MDBID_LERR_ LERR_ << MongoDBIndexDriver_LOG_HEAD << __FUNCTION__ << " - "



MongoDBIndexDriver::MongoDBIndexDriver(std::string alias):IndexDriver(alias) {
	
}


MongoDBIndexDriver::~MongoDBIndexDriver() {
	
}

//! init
void MongoDBIndexDriver::init(void *init_data) throw (chaos::CException) {
	IndexDriver::init(init_data);
	std::string errmsg;
	std::string servers;
	chaos_data::CDataWrapper driver_custom_init;
	//allcoate ha pool class
	ha_connection_pool = new MongoDBHAConnectionManager(setting->servers, setting->key_value_custom_param);
}

//!deinit
void MongoDBIndexDriver::deinit() throw (chaos::CException) {
	IndexDriver::deinit();
	if(ha_connection_pool) delete(ha_connection_pool);
}

//! Register a new domain
int MongoDBIndexDriver::vfsAddDomain(vfs::VFSDomain domain) {
	int err = 0;
	mongo::BSONObjBuilder b;
	try {
		//compose file search criteria
		b.append(MONGO_DB_FIELD_DOMAIN_NAME, domain.name);
		b.append(MONGO_DB_FIELD_DOMAIN_URL, domain.local_url);
		b.append(MONGO_DB_FIELD_DOMAIN_HB, mongo::Date_t(chaos::TimingUtil::getTimeStamp()));
		err = ha_connection_pool->insert(MONGO_DB_VFS_DOMAINS_COLLECTION, b.obj());
		if(err) {
			if(err != 11000) {
				MDBID_LERR_ << "Error " << err << " creting domain entry";
			} else {
				err = vfsDomainHeartBeat(domain);
			}
		}
		
	} catch (const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -5;
	}
	return err;
}

//! Give an heart beat for a domain
int MongoDBIndexDriver::vfsDomainHeartBeat(vfs::VFSDomain domain) {
	int err = 0;
	mongo::BSONObjBuilder b_query;
	mongo::BSONObjBuilder b_update_filed;
	mongo::BSONObjBuilder b_update;
	try {
		//compose file search criteria
		b_query.append(MONGO_DB_FIELD_DOMAIN_NAME, domain.name);
		b_query.append(MONGO_DB_FIELD_DOMAIN_URL, domain.local_url);
		b_update_filed.append(MONGO_DB_FIELD_DOMAIN_HB, mongo::Date_t(chaos::TimingUtil::getTimeStamp()));
		b_update.append("$set", b_update_filed.obj());
		err = ha_connection_pool->update(MONGO_DB_VFS_DOMAINS_COLLECTION, b_query.obj(), b_update.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating the heartbeat for domain";
		}
		
	} catch (const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -5;
	}
	return err;
}

//! Register a new data block wrote on stage area
int MongoDBIndexDriver::vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file,
										   chaos_vfs::DataBlock *data_block,
										   vfs::data_block_state::DataBlockState new_block_state) {
	int err = 0;
	bool f_exists = false;
	
	if(vfsFileExist(vfs_file, f_exists)) {
		MDBID_LERR_ << "Error checking file description of vfat";
		return -1;
	}
	
	if(!f_exists) {
		if(vfsCreateFileEntry(vfs_file)) {
			MDBID_LERR_ << "Error creating file description of vfat";
			return -2;
		}
	}
	
	//allocate data block on vfat
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block;
	mongo::BSONObj search_result;
	try {
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		ha_connection_pool->findOne(search_result, MONGO_DB_VFS_VFAT_COLLECTION, bson_search.obj());
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "Error getting file information";
			return -4;
		}
		
		//insert new
		bson_block.append(MONGO_DB_FIELD_FILE_PRIMARY_KEY, search_result["_id"].OID());
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_STATE, new_block_state);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS, mongo::Date_t(data_block->creation_time));
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS, mongo::Date_t(data_block->invalidation_timestamp));
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE, (long long)data_block->max_reacheable_size);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH, data_block->vfs_path);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		ha_connection_pool->insert(MONGO_DB_VFS_VBLOCK_COLLECTION,  bson_block.obj());
		
	} catch (const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -5;
	}
	return err;
}

//! Set the state for a stage datablock
int MongoDBIndexDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block,
											   vfs::data_block_state::DataBlockState state) {
	int err = 0;
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block_query;
	mongo::BSONObjBuilder bson_block_update;
	mongo::BSONObjBuilder bson_block_update_filed;
	mongo::BSONObj search_result;
	try{
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		err = ha_connection_pool->findOne(search_result, MONGO_DB_VFS_VFAT_COLLECTION, bson_search.obj());
		
		if(err) {
			MDBID_LERR_ << "Error " << err << " searching vfs on vfat";
			return err;
		}
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "[CAN'T BE HERE]Error getting file information";
			
		}
		
		//compose query
		bson_block_query.append(MONGO_DB_FIELD_FILE_PRIMARY_KEY, search_result["_id"].OID());
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS, mongo::Date_t(data_block->creation_time));
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH, data_block->vfs_path);
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		//compose udpate
		bson_block_update_filed.append(MONGO_DB_FIELD_DATA_BLOCK_STATE, state);
		bson_block_update.append("$set", bson_block_update_filed.obj());
		
		err = ha_connection_pool->update(MONGO_DB_VFS_VBLOCK_COLLECTION, bson_block_query.obj(), bson_block_update.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating state on datablock";
		}
	}catch(const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -2;
	}
	return err;
}

//! Set the state for a stage datablock
int MongoDBIndexDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
											   chaos_vfs::DataBlock *data_block,
											   vfs::data_block_state::DataBlockState cur_state,
											   vfs::data_block_state::DataBlockState new_state) {
	int err = 0;
	mongo::BSONObjBuilder command;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObjBuilder query_cond;
	mongo::BSONObjBuilder update;
	mongo::BSONObj result;
	try{
		//domain search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		//path search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << vfs_file->getVFSFileInfo()->vfs_fpath;
		//query on state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << cur_state;
		
		//start the find and modify command
		command << "findandmodify" << MONGO_DB_VFS_VBLOCK_COLLECTION;
		//compose file search criteria
		command << "query" << query_master.obj();
		//set the atomic update on the tate
		command << "update" << BSON("$set" << BSON( MONGO_DB_FIELD_DATA_BLOCK_STATE << new_state) );
		
		err = ha_connection_pool->runCommand(result, MONGO_DB_VFS_VFAT_COLLECTION, query_master.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
			err = -1;
		} else {
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Return the next available datablock created since timestamp
int MongoDBIndexDriver::vfsFindSinceTimeDataBlock(chaos_vfs::VFSFile *vfs_file,
												  uint64_t timestamp,
												  bool direction,
												  data_block_state::DataBlockState state,
												  chaos_vfs::DataBlock **data_block) {
	int err = 0;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObj result;
	try{
		//search for domain
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		//search on file path, the datablock is always the end token of the path
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << BSON("$regex" << "stage/eb6b929d/thread_0/.*");

		//search for state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << state;
		//search on the timestamp
		query_master << "$gt" << BSON(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS << mongo::Date_t(timestamp));
		
		err = ha_connection_pool->findOne(result, MONGO_DB_VFS_VFAT_COLLECTION, query_master.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
		} else {
			// read the block element
			
			//get new empty datablock
			*data_block = getEmptyDataBlock();
			
			//set the field
			(*data_block)->creation_time = result.getField(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS).numberLong();
			(*data_block)->invalidation_timestamp = result.getField(MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS).numberLong();
			(*data_block)->max_reacheable_size = result.getField(MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE).numberLong();
			
			std::string path = result.getField(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH).String();
			(*data_block)->vfs_path = (char*)malloc(sizeof(char) * path.size()+1);
			if((*data_block)->vfs_path) {
				::strcpy((*data_block)->vfs_path , path.c_str());
			} else {
				deleteDataBlock(*data_block);
				*data_block = NULL;
				err = -1;
			}
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Heartbeat update stage block
int MongoDBIndexDriver::vfsWorkHeartBeatOnDataBlock(chaos_vfs::VFSFile *vfs_file,
													chaos_vfs::DataBlock *data_block) {
	return 0;
}

//! Check if the vfs file exists
int MongoDBIndexDriver::vfsFileExist(VFSFile *vfs_file, bool& exists_flag) {
	int err = 0;
	mongo::BSONObjBuilder b;
	mongo::BSONObj result;
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		err = ha_connection_pool->findOne(result, MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else {
			exists_flag = !result.isEmpty();
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Create a file entry into the vfat
int MongoDBIndexDriver::vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file) {
	int err = 0;
	mongo::BSONObjBuilder b;
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		err = ha_connection_pool->insert(MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
		if(err) {
			if(err != 11000) {
				MDBID_LERR_ << "Error " << err << " creting vfs file entry";
			} else {
				err = 0;
			}
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Return a list of vfs path of the file belong to a domain
int MongoDBIndexDriver::vfsGetFilePathForDomain(std::string vfs_domain, std::string prefix_filter, std::vector<std::string>& result_vfs_file_path, int limit_to_size) {
	int err = 0;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObjBuilder return_field;
	std::vector<mongo::BSONObj> results;
	try{
		//search for domain
		query_master << MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_domain;
		
		//after the prefix we need to add the regex
		prefix_filter.append(".*");
		
		query_master << MONGO_DB_FIELD_FILE_VFS_PATH << BSON("$regex" << prefix_filter);
		
		return_field << MONGO_DB_FIELD_FILE_VFS_PATH << 1 << "_id" << 0;
		
		mongo::BSONObj q_obj = query_master.obj();
		mongo::BSONObj f_obj = return_field.obj();
		
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForDomain query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Domain: " << vfs_domain;)
		DEBUG_CODE(MDBID_LDBG_ << "Prefix: " << prefix_filter;)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q_obj.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "Selected Fileds: "  << f_obj.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForDomain query ---------------------------------------------";)
		
		ha_connection_pool->findN(results, MONGO_DB_VFS_VFAT_COLLECTION, q_obj, limit_to_size, 0, &f_obj);
		for(std::vector<mongo::BSONObj>::iterator it = results.begin();
			it != results.end();
			it++) {
			result_vfs_file_path.push_back(it->getField(MONGO_DB_FIELD_FILE_VFS_PATH));
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}