//
//  MongoDBDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MongoDBDriver.h"
#include "MongoDBIndexCursor.h"

#include "../vfs/vfs.h"

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::data_service::vfs;
using namespace chaos::data_service::db_system;
using namespace chaos;

namespace chaos_data = chaos::common::data;

#define MongoDBDriver_LOG_HEAD "[MongoDBDriver] - "
#define MDBID_LAPP_ LAPP_ << MongoDBDriver_LOG_HEAD
#define MDBID_LDBG_ LDBG_ << MongoDBDriver_LOG_HEAD << __FUNCTION__ << " - "
#define MDBID_LERR_ LERR_ << MongoDBDriver_LOG_HEAD << __FUNCTION__ << " - "


//!
MongoDBDriver::MongoDBDriver(std::string alias):DBDriver(alias) {}

//!
MongoDBDriver::~MongoDBDriver() {}

//! init
void MongoDBDriver::init(void *init_data) throw (chaos::CException) {
	DBDriver::init(init_data);
	int err = 0;
	std::string errmsg;
	std::string servers;
	chaos_data::CDataWrapper driver_custom_init;
	//allcoate ha pool class
	ha_connection_pool = new MongoDBHAConnectionManager(setting->servers, setting->key_value_custom_param);
	if(setting->key_value_custom_param.count("db")) {
		db_name = setting->key_value_custom_param["db"];
	} else {
		db_name = MONGO_DB_NAME;
	}
	//setup index
	//db.vdomain.ensureIndex( { "vfs_domain":1, "vfs_unique_domain_code":1 } , { unique: true,  dropDups: true  } )
	//db.vfat.ensureIndex( { "vfs_path": 1, "vfs_domain":1 } , { unique: true,  dropDups: true  } )
	//db.runCommand( { shardCollection : "chaos_vfs.domains" , key : { domain_name: 1, domain_url:1 } , unique : true,  dropDups: true  } )
	
	//domain index
	mongo::BSONObj index_on_domain = BSON(MONGO_DB_FIELD_DOMAIN_NAME<<1<<
										  MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE<<1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating domain collection index", __PRETTY_FUNCTION__);
	
	//domain url index
	index_on_domain = BSON(MONGO_DB_FIELD_DOMAIN_NAME<<1<<
						   MONGO_DB_FIELD_DOMAIN_URL<<1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS_URL, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating domain urls collection index", __PRETTY_FUNCTION__);
	
	index_on_domain = BSON(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH<<1<<
						   MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN<<1<<
						   MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS<<1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating data block index", __PRETTY_FUNCTION__);
	
	index_on_domain = BSON(MONGO_DB_FIELD_IDX_DATA_PACK_DID<<1<<
						   MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS<<1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating data pack index collection index", __PRETTY_FUNCTION__);
	
	index_on_domain = BSON(MONGO_DB_FIELD_SNAPSHOT_NAME<< 1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_SNAPSHOT, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating snapshot collection index", __PRETTY_FUNCTION__);
	
	index_on_domain = BSON(MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << 1 <<
						   MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << 1 <<
						   MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID << 1);
	err = ha_connection_pool->ensureIndex(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA, index_on_domain, true, "", true);
	if(err) throw chaos::CException(-1, "Error creating snapshot data collection index", __PRETTY_FUNCTION__);
	
}

//!deinit
void MongoDBDriver::deinit() throw (chaos::CException) {
	DBDriver::deinit();
	if(ha_connection_pool) delete(ha_connection_pool);
}

//! Register a new domain
int MongoDBDriver::vfsAddDomain(vfs::VFSDomain domain) {
	int err = 0;
	mongo::BSONObjBuilder domain_registration;
	mongo::BSONObjBuilder domain_url_registration;
	try {
		//try to add the domain
		domain_registration.append(MONGO_DB_FIELD_DOMAIN_NAME, domain.name);
		domain_registration.append(MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE, domain.unique_code);
		mongo::BSONObj domain_insertation_obj = domain_registration.obj();
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS), domain_insertation_obj);
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting domain entry";
			MDBID_LAPP_ << "Checking already stored domain info";
			mongo::BSONObj result;
			// no i can read the memoryzed unique code for my domain and see if it match
			err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS), domain_insertation_obj);
			if(err) {
				MDBID_LERR_ << "Error " << err << " retriving domain info";
			} else if(result.isEmpty()) {
				MDBID_LERR_ << "No domain found on index db --> somenthing is going bad on db";
				err = -1;
				return err;
			} else {
				MDBID_LAPP_ << "Checking domain unique code";
				MDBID_LAPP_ << "Local unique domain code->" << domain.unique_code;
				MDBID_LAPP_ << "Index db unique domaincode->" << result.getStringField(MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE);
				if(domain.unique_code.compare(result.getStringField(MONGO_DB_FIELD_DOMAIN_UNIQUE_CODE))) {
					MDBID_LERR_ << "Unique code checking NOT PASSED";
					err = -2;
					return err;
				}
			}
		}
		//compose the insert
		domain_url_registration.append(MONGO_DB_FIELD_DOMAIN_NAME, domain.name);
		domain_url_registration.append(MONGO_DB_FIELD_DOMAIN_URL, domain.local_url);
		domain_url_registration.append(MONGO_DB_FIELD_DOMAIN_HB, mongo::Date_t(chaos::TimingUtil::getTimeStamp()));
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS_URL), domain_url_registration.obj());
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
int MongoDBDriver::vfsDomainHeartBeat(vfs::VFSDomain domain) {
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
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_DOMAINS_URL), b_query.obj(), b_update.obj());
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
int MongoDBDriver::vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file,
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
	
	mongo::BSONObjBuilder search_id;
	mongo::BSONObjBuilder search_id_return_field;
	
	mongo::BSONObj search_result;
	try {
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		ha_connection_pool->findOne(search_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), bson_search.obj());
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
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, (data_block->vfs_domain = vfs_file->getVFSFileInfo()->vfs_domain));
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_CURRENT_WORK_POSITION, (long long)0);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_HB, mongo::Date_t(chaos::TimingUtil::getTimeStamp()));
		
		ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK),  bson_block.obj());
		
		//collecte the unique id of the datablock
		search_id << MONGO_DB_FIELD_FILE_VFS_PATH << data_block->vfs_path;
		search_id << MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		//set the result needed field
		search_id_return_field << "_id" << 1;
		mongo::BSONObj o = search_id_return_field.obj();
		
		ha_connection_pool->findOne(search_result,
									MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK),
									search_id.obj(),
									&o);
		
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "Error getting file information";
			return -5;
		}
		
		//retrive the id
		data_block->index_driver_uinique_id = search_result["_id"].OID().toString();
	} catch (const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -5;
	}
	return err;
}

//! Delete a virtual file datablock
int MongoDBDriver::vfsDeleteDataBlock(chaos_vfs::VFSFile *vfs_file,
									  chaos_vfs::DataBlock *data_block) {
	int err = 0;
	mongo::BSONObjBuilder file_search;
	mongo::BSONObj file_search_result;
	mongo::BSONObjBuilder data_block_search;
	try{
		//add default index information
		file_search << MONGO_DB_FIELD_FILE_VFS_PATH << vfs_file->getVFSFileInfo()->vfs_fpath;
		file_search << MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		ha_connection_pool->findOne(file_search_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), file_search.obj());
		if(file_search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "Error getting file information";
			return -4;
		}
		
		//insert new
		data_block_search << MONGO_DB_FIELD_FILE_PRIMARY_KEY << file_search_result["_id"].OID();
		data_block_search << MONGO_DB_FIELD_FILE_VFS_PATH << data_block->vfs_path;
		data_block_search << MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		mongo::BSONObj q = data_block_search.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsDeleteDataBlock delete ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "vfsDeleteDataBlock delete ---------------------------------------------";)
		
		err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), q);
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Set the state for a stage datablock
int MongoDBDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
										  chaos_vfs::DataBlock *data_block,
										  int state) {
	int err = 0;
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block_query;
	mongo::BSONObjBuilder bson_block_update;
	mongo::BSONObjBuilder bson_block_update_filed;
	mongo::BSONObj search_result;
	try{
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		err = ha_connection_pool->findOne(search_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), bson_search.obj());
		
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
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH, data_block->vfs_path);
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		//compose udpate
		bson_block_update_filed.append(MONGO_DB_FIELD_DATA_BLOCK_STATE, state);
		bson_block_update.append("$set", bson_block_update_filed.obj());
		
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), bson_block_query.obj(), bson_block_update.obj());
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
int MongoDBDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file,
										  chaos_vfs::DataBlock *data_block,
										  int cur_state,
										  int new_state,
										  bool& success) {
	int err = 0;
	mongo::BSONObjBuilder command;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObj result;
	success = false;
	try{
		//domain search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		//path search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << data_block->vfs_path;
		//query on state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << cur_state;
		
		//start the find and modify command
		command << "findAndModify" << MONGO_DB_COLLECTION_VFS_VBLOCK;
		//compose file search criteria
		command << "query" << query_master.obj();
		//set the atomic update on the tate
		command << "update" << BSON("$set" << BSON( MONGO_DB_FIELD_DATA_BLOCK_STATE << new_state) );
		
		mongo::BSONObj q = command.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsSetStateOnDataBlock query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsSetStateOnDataBlock query ---------------------------------------------";)
		
		err = ha_connection_pool->runCommand(result, db_name, q);
		if(err) {
			MDBID_LERR_ << "Error " << err << " vfsSetStateOnDataBlock";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
			err = -1;
		} else {
			success = true;
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Set the datablock current position
int MongoDBDriver::vfsSetHeartbeatOnDatablock(chaos_vfs::VFSFile *vfs_file,
											  chaos_vfs::DataBlock *data_block,
											  uint64_t timestamp) {
	int err = 0;
	mongo::BSONObjBuilder bson_block_query;
	mongo::BSONObjBuilder bson_block_update;
	try{
		bson_block_query << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << data_block->vfs_path;
		bson_block_query << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		//compose udpate
		bson_block_update << "$set"<< BSON(MONGO_DB_FIELD_DATA_BLOCK_HB << mongo::Date_t(((timestamp == 0)?chaos::TimingUtil::getTimeStamp():timestamp)));
		
		//for heart beat we use unsecure write
		mongo::BSONObj q = bson_block_query.obj();
		mongo::WriteConcern wc = mongo::WriteConcern::unacknowledged;
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), q, bson_block_update.obj(), &wc);
		DEBUG_CODE(MDBID_LDBG_ << "vfsSetHeartbeatOnDatablock query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsSetHeartbeatOnDatablock query ---------------------------------------------";)
		
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating state on datablock";
		}
	}catch(const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -2;
	}
	return err;
}

//! Set the datablock current position
int MongoDBDriver::vfsUpdateDatablockCurrentWorkPosition(chaos_vfs::VFSFile *vfs_file,
														 chaos_vfs::DataBlock *data_block) {
	int err = 0;
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block_query;
	mongo::BSONObjBuilder bson_block_update;
	mongo::BSONObj search_result;
	try{
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		err = ha_connection_pool->findOne(search_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), bson_search.obj());
		
		if(err) {
			MDBID_LERR_ << "Error " << err << " searching vfs on vfat";
			return err;
		}
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "[CAN'T BE HERE]Error getting file information";
			
		}
		
		//compose query
		bson_block_query << MONGO_DB_FIELD_FILE_PRIMARY_KEY << search_result["_id"].OID();
		bson_block_query << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << data_block->vfs_path;
		bson_block_query << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		//compose udpate
		bson_block_update << "$set"<< BSON(MONGO_DB_FIELD_DATA_BLOCK_CURRENT_WORK_POSITION << (long long)data_block->current_work_position);
		
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), bson_block_query.obj(), bson_block_update.obj());
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating state on datablock";
		}
	}catch(const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -2;
	}
	return err;
}

//! fill a complete datablock
chaos_vfs::DataBlock *MongoDBDriver::fillDatablock(const mongo::BSONObj& full_datablock_query_result, chaos_vfs::DataBlock *data_block) {
	if(!data_block) return data_block;
	
	//set the field
	(*data_block).index_driver_uinique_id = full_datablock_query_result["_id"].OID().toString();
	(*data_block).creation_time = full_datablock_query_result.getField(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS).numberLong();
	(*data_block).invalidation_timestamp = full_datablock_query_result.getField(MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS).numberLong();
	(*data_block).max_reacheable_size = full_datablock_query_result.getField(MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE).numberLong();
	(*data_block).vfs_domain = full_datablock_query_result.getField(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN).String();
	(*data_block).vfs_path = full_datablock_query_result.getField(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH).String();
	return data_block;
}

//! return the path of the vfile that contains the oldest block with info
int MongoDBDriver::vfsGetFilePathForOldestBlockState(const std::string& domain,
													 const std::string& data_area,
													 int state,
													 std::string& vfile_path) {
	int err = 0;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObj result;
	try{
		//search for domain
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << domain;
		
		//search on file path, the datablock is always the end token of the path
		
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << BSON("$regex" << boost::str(boost::format("%1%%2%") % data_area % ".*"));
		
		//search for state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << state;
		//search on the timestamp
		query_master << MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS << mongo::BSONObjBuilder().appendDate("$gt", TimingUtil::getTimestampFromString("1970-01-01")).obj();
		
		mongo::BSONObj q = query_master.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForOldestBlockState query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForOldestBlockState query ---------------------------------------------";)
		
		mongo::Query mongo_query = q;
		mongo_query.sort(BSON(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS << 1));
		err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), mongo_query);
		if(err) {
			MDBID_LERR_ << "Error " << err << " searching oldest datablock with state";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
		} else if(result.hasField(MONGO_DB_FIELD_FILE_PRIMARY_KEY)){
			mongo::BSONObjBuilder file_query;
			file_query << "_id" << result[MONGO_DB_FIELD_FILE_PRIMARY_KEY].OID();
			q = file_query.obj();
			DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForOldestBlockState 2nd query ---------------------------------------------";)
			DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
			DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForOldestBlockState 2nd query ---------------------------------------------";)
			err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), q);
			if(err) {
				MDBID_LERR_ << "Error " << err << " searching file description";
			} else if(result.isEmpty()){
				MDBID_LERR_ << "file description found";
			} else if(result.hasField(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH)){
				vfile_path = result.getStringField(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH);
			}
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;

}

//! Change the state to all datablock that are in timeout
int MongoDBDriver::vfsChangeStateToOutdatedChunck(const std::string& domain,
												  const std::string& data_area,
												  uint32_t timeout_state_in_sec,
												  int timeout_state,
												  int new_state) {
	int err = 0;
	mongo::BSONObjBuilder update;
	mongo::BSONObjBuilder query_master;
	try{
		//domain search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << domain;
		//path search
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << BSON("$regex" << boost::str(boost::format("%1%%2%") % data_area % ".*"));
		//query on state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << timeout_state;
		
		//search on the timestamp
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS << mongo::BSONObjBuilder().appendDate("$lt", TimingUtil::getTimestampWithDelay(timeout_state_in_sec*1000)).obj();

		//set the atomic update on the tate
		update<< "$set" << BSON( MONGO_DB_FIELD_DATA_BLOCK_STATE << new_state);
		
		mongo::BSONObj q = query_master.obj();
		mongo::BSONObj u = update.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsChangeStateToOutdatedChunck update ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "Update: "  << u.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsChangeStateToOutdatedChunck update ---------------------------------------------";)
		
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), q, u, false, true);
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating state on all datablock in timeout";
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Return the next available datablock created since timestamp
int MongoDBDriver::vfsFindSinceTimeDataBlock(chaos_vfs::VFSFile *vfs_file,
											 uint64_t timestamp,
											 bool direction,
											 int state,
											 chaos_vfs::DataBlock **data_block) {
	int err = 0;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObj result;
	try{
		//search for domain
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << vfs_file->getVFSFileInfo()->vfs_domain;
		
		//search on file path, the datablock is always the end token of the path
		
		query_master << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << BSON("$regex" << boost::str(boost::format("%1%%2%") % vfs_file->getVFSFileInfo()->vfs_fpath % ".*"));
		
		//search for state
		query_master << MONGO_DB_FIELD_DATA_BLOCK_STATE << state;
		//search on the timestamp
		query_master << MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS << mongo::BSONObjBuilder().appendDate("$gt", timestamp).obj();
		
		mongo::BSONObj q = query_master.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsFindSinceTimeDataBlock query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsFindSinceTimeDataBlock query ---------------------------------------------";)
		
		
		err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), q);
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
		} else {
			// read the block element
			
			//get new empty datablock
			*data_block = fillDatablock(result, new chaos_vfs::DataBlock());
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Return the datablock identified by path
int MongoDBDriver::vfsFindFromPathDataBlock(const std::string& data_block_domain,
											const std::string& data_block_path,
											chaos_vfs::DataBlock **data_block) {
	int err = 0;
	mongo::BSONObj result;
	mongo::BSONObjBuilder query_data_blocks;
	try{
		//search for domain
		query_data_blocks << MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN << data_block_domain;
		
		//search on file path, the datablock is always the end token of the path
		
		query_data_blocks << MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH << data_block_path;
		
		//search for state
		query_data_blocks << MONGO_DB_FIELD_DATA_BLOCK_STATE << chaos_vfs::data_block_state::DataBlockStateQuerable;
		
		mongo::BSONObj q = query_data_blocks.obj();
		DEBUG_CODE(MDBID_LDBG_ << "vfsFindFromPathDataBlock query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsFindFromPathDataBlock query ---------------------------------------------";)
		
		if((err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK), q))) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else if(result.isEmpty()){
			MDBID_LERR_ << "No datablock found for the criteria";
		} else {
			//get new empty datablock
			*data_block = fillDatablock(result, new chaos_vfs::DataBlock());
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Check if the vfs file exists
int MongoDBDriver::vfsFileExist(VFSFile *vfs_file, bool& exists_flag) {
	int err = 0;
	mongo::BSONObjBuilder b;
	mongo::BSONObj result;
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), b.obj());
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
int MongoDBDriver::vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file) {
	int err = 0;
	mongo::BSONObjBuilder b;
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), b.obj());
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
int MongoDBDriver::vfsGetFilePathForDomain(const std::string& vfs_domain,
										   const std::string& prefix_filter,
										   std::vector<std::string>& result_vfs_file_path,
										   int limit_to_size) {
	int err = 0;
	mongo::BSONObjBuilder query_master;
	mongo::BSONObjBuilder return_field;
	std::vector<mongo::BSONObj> results;
	try{
		std::string _prefix_filter = prefix_filter;
		//after the prefix we need to add the regex
		_prefix_filter.append(".*");
		
		query_master << "$query" << BSON(MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_domain << MONGO_DB_FIELD_FILE_VFS_PATH << BSON("$regex" << _prefix_filter))
		<< "$orderby" << BSON(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS << 1);
		//search for domain
		//query_master << MONGO_DB_FIELD_FILE_VFS_DOMAIN << vfs_domain;
		
		
		//query_master << MONGO_DB_FIELD_FILE_VFS_PATH << BSON("$regex" << _prefix_filter);
		
		return_field << MONGO_DB_FIELD_FILE_VFS_PATH << 1 << "_id" << 0;
		
		mongo::BSONObj q_obj = query_master.obj();
		mongo::BSONObj f_obj = return_field.obj();
		
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForDomain query ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "Domain: " << vfs_domain;)
		DEBUG_CODE(MDBID_LDBG_ << "Prefix: " << _prefix_filter;)
		DEBUG_CODE(MDBID_LDBG_ << "Query: "  << q_obj.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "Selected Fileds: "  << f_obj.jsonString();)
		DEBUG_CODE(MDBID_LDBG_ << "vfsGetFilePathForDomain query ---------------------------------------------";)
		
		ha_connection_pool->findN(results, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VFAT), q_obj, limit_to_size, 0, &f_obj);
		for(std::vector<mongo::BSONObj>::iterator it = results.begin();
			it != results.end();
			it++) {
			result_vfs_file_path.push_back(it->getField(MONGO_DB_FIELD_FILE_VFS_PATH).String());
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! add the default index for a unique instrument identification and a timestamp
int MongoDBDriver::idxAddDataPackIndex(const DataPackIndex& index) {
	int err = 0;
	mongo::BSONObjBuilder index_builder;
	mongo::BSONObjBuilder index_data_block_info_builder;
	try{
		//add default index information
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DID << index.did;
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS << mongo::Date_t(index.acquisition_ts);
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC << (long long)index.acquisition_ts;
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN << getDataBlockFromFileLocation(index.dst_location)->vfs_domain;
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_PATH << getDataBlockFromFileLocation(index.dst_location)->vfs_path;
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET << (long long)getDataBlockOffsetFromFileLocation(index.dst_location);
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_SIZE << (int32_t)index.datapack_size;
		index_builder << MONGO_DB_FIELD_IDX_DATA_PACK_STATE << (int32_t)DataPackIndexQueryStateCreated;
		
		DEBUG_CODE(MDBID_LDBG_ << "idxAddDataPackIndex insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "did: " << index.did;)
		DEBUG_CODE(MDBID_LDBG_ << "acq_ts_numeric: " << index.acquisition_ts;)
		DEBUG_CODE(MDBID_LDBG_ << "data_pack: " << getDataBlockFromFileLocation(index.dst_location)->vfs_path;)
		DEBUG_CODE(MDBID_LDBG_ << "data_pack_offset: " << getDataBlockOffsetFromFileLocation(index.dst_location);)
		DEBUG_CODE(MDBID_LDBG_ << "idxAddDataPackIndex insert ---------------------------------------------";)
		
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), index_builder.obj());
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = e.getCode();
	}
	return err;
}

//! add the default index for a unique instrument identification and a timestamp
int MongoDBDriver::idxDeleteDataPackIndex(const DataPackIndex& index) {
	int err = 0;
	mongo::BSONObjBuilder index_search_builder;
	try{
		//add default index information
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DID << index.did;
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS << mongo::Date_t(index.acquisition_ts);
		mongo::BSONObj q = index_search_builder.obj();
		DEBUG_CODE(MDBID_LDBG_ << "idxDeleteDataPackIndex insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "idxDeleteDataPackIndex insert ---------------------------------------------";)
		
		err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), q);
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! set the state on all datapack index for an datablock
int MongoDBDriver::idxSetDataPackIndexStateByDataBlock(const std::string& vfs_datablock_domain,
													   const std::string& vfs_datablock_path,
													   DataPackIndexQueryState dp_index_state) {
	int err = 0;
	mongo::BSONObjBuilder index_search_builder;
	mongo::BSONObjBuilder bson_block_update;
	try{
		//add default index information
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN << vfs_datablock_domain;
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_PATH << vfs_datablock_path;
		
		bson_block_update << "$set"<< BSON(MONGO_DB_FIELD_IDX_DATA_PACK_STATE << (int32_t)dp_index_state);

		mongo::BSONObj q = index_search_builder.obj();
		mongo::BSONObj u = bson_block_update.obj();
		DEBUG_CODE(MDBID_LDBG_ << "idxSetDataPackIndexStateByDataBlock update ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "update: " << u.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "idxSetDataPackIndexStateByDataBlock update ---------------------------------------------";)
		
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), q, u, false, true, &mongo::WriteConcern::acknowledged);
		if(err) {
			MDBID_LERR_ << "Error " << err << " updating state on all datablock index";
		}
		
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! perform a search on data pack indexes
int MongoDBDriver::idxStartSearchDataPack(const DataPackIndexQuery& data_pack_index_query, DBIndexCursor **index_cursor) {
	int err=-1;
	*index_cursor = new MongoDBIndexCursor(this, data_pack_index_query);
	if(*index_cursor) {
		err = ((MongoDBIndexCursor*)*index_cursor)->computeTimeLapsForPage();
	}
	return err;
}

#pragma mark Protected Query Function
//-------------------------- protected method------------------------------
//! protected methdo that perform the real paged query on index called by the cursor
int MongoDBDriver::idxSearchResultCountDataPack(const DataPackIndexQuery& data_pack_index_query,  uint64_t& num_of_result) {
	int err = 0;
	mongo::BSONObjBuilder	index_search_builder;
	mongo::BSONObjBuilder	return_field;
	try{
		//add default index information
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DID << data_pack_index_query.did;
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_STATE << (int32_t)DataPackIndexQueryStateQuerable; //select only querable indexes
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC << BSON("$gte" << (long long)data_pack_index_query.start_ts <<
																					"$lte" << (long long)data_pack_index_query.end_ts);
		
		mongo::BSONObj q = index_search_builder.obj();
		DEBUG_CODE(MDBID_LDBG_ << "idxSearchResultCountDataPack insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "idxSearchResultCountDataPack insert ---------------------------------------------";)
		
		mongo::BSONObj r = return_field.obj();
#if __GNUC__
		err = ha_connection_pool->count((unsigned long long &)num_of_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), q);
#else
		err = ha_connection_pool->count(num_of_result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_FIELD_IDX_DATA_PACK), q);
#endif // __GNUC__
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! perform a search on data pack indexes
int MongoDBDriver::idxSearchDataPack(const DataPackIndexQuery& data_pack_index_query, std::vector<mongo::BSONObj>& found_element, uint32_t limit_to) {
	int err = 0;
	mongo::BSONObjBuilder	index_search_builder;
	mongo::BSONObjBuilder	return_field;
	try{
		//add default index information
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DID << data_pack_index_query.did;
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_STATE << (int32_t)DataPackIndexQueryStateQuerable; //select only querable indexes
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC << BSON("$gte" << (long long)data_pack_index_query.start_ts);
		
		//set the field to return
		return_field << MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_DOMAIN << 1
		<< MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_PATH << 1
		<< MONGO_DB_FIELD_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET << 1
		<< MONGO_DB_FIELD_IDX_DATA_PACK_SIZE << 1
		<< MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC << 1;
		
		mongo::BSONObj q = index_search_builder.obj();
		mongo::BSONObj r = return_field.obj();
		DEBUG_CODE(MDBID_LDBG_ << "idxDeleteDataPackIndex insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "idxDeleteDataPackIndex insert ---------------------------------------------";)
		
		ha_connection_pool->findN(found_element, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), q, limit_to, 0, &r);
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! protected methdo that perform the real paged query on index called by the cursor
int MongoDBDriver::idxMaxAndMInimumTimeStampForDataPack(const DataPackIndexQuery & data_pack_index_query, uint64_t& min_ts, uint64_t& max_ts) {
	int err = 0;
	mongo::BSONObj r_max;
	mongo::BSONObj r_min;
	mongo::BSONObjBuilder	return_field;
	mongo::BSONObjBuilder	index_search_builder;
	try{
		return_field << MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC << 1;
		
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_DID << data_pack_index_query.did;
		index_search_builder << MONGO_DB_FIELD_IDX_DATA_PACK_STATE << (int32_t)DataPackIndexQueryStateQuerable; //select only querable indexes
		
		mongo::BSONObj p = return_field.obj();
		mongo::BSONObj q = index_search_builder.obj();
		DEBUG_CODE(MDBID_LDBG_ << "idxMaxAndMInimumTimeStampForDataPack insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "query: " << q.jsonString());
		DEBUG_CODE(MDBID_LDBG_ << "idxMaxAndMInimumTimeStampForDataPack insert ---------------------------------------------";)
		
		if((err = ha_connection_pool->findOne(r_max, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), mongo::Query(q).sort(MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC,-1), &p))){
			MDBID_LERR_ << "Error getting maximum timestamp on index of " << data_pack_index_query.did;
		} else if((err = ha_connection_pool->findOne(r_min, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_IDX_DATA_PACK), mongo::Query(q).sort(MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC,1), &p))) {
			MDBID_LERR_ << "Error getting minimum timestamp on index of " << data_pack_index_query.did;
		} else if(r_min.isEmpty() || r_max.isEmpty()) {
			MDBID_LERR_ << "No max or min found for " << data_pack_index_query.did;
			err = -1000;
		} else {
			min_ts = (uint64_t)r_min.getField(MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC).Long();
			max_ts = (uint64_t)r_max.getField(MONGO_DB_FIELD_IDX_DATA_PACK_ACQ_TS_NUMERIC).Long();
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Create a new snapshot
int MongoDBDriver::snapshotCreateNewWithName(const std::string& snapshot_name,
											 std::string& working_job_unique_id) {
	int err = 0;
	mongo::BSONObjBuilder	new_snapshot_start;
	
	//----- generate the random code ------ for this snapshot
	working_job_unique_id = UUIDUtil::generateUUIDLite();
	try{
		new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_NAME << snapshot_name;
		new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_TS << mongo::Date_t(TimingUtil::getTimeStamp());
		new_snapshot_start << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
		
		mongo::BSONObj q = new_snapshot_start.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q);
		if(err == 11000) {
			//already exis a snapshot with taht name so no error need to be throw
			err = 1;
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Add an element to a named snapshot
int MongoDBDriver::snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
												const std::string& snapshot_name,
												const std::string& producer_unique_key,
												const std::string& dataset_type,
												void* data,
												uint32_t data_len) {
	int err = 0;
	mongo::BSONObjBuilder	new_dataset;
	mongo::BSONObjBuilder	search_snapshot;
	try{
		new_dataset << "$set"<< BSON(dataset_type << mongo::BSONObj((const char *)data));
		
		//search for snapshot name and producer unique key
		search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
		search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID << producer_unique_key;
		search_snapshot << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
		
		mongo::BSONObj u = new_dataset.obj();
		mongo::BSONObj q = search_snapshot.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "update:" << u;)
		DEBUG_CODE(MDBID_LDBG_ << "condition" << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		
		//update and waith until the data is on the server
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q, u, true, false, &mongo::WriteConcern::acknowledged);
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

int MongoDBDriver::snapshotIncrementJobCounter(const std::string& working_job_unique_id,
											   const std::string& snapshot_name,
											   bool add) {
	int err = 0;
	mongo::BSONObjBuilder	inc_update;
	mongo::BSONObjBuilder	search_snapshot;
	try{
		inc_update << "$inc"<< BSON(MONGO_DB_FIELD_SNAPSHOT_JOB_CONCURRENCY<< (add?1:-1));
		
		//search for snapshot name and producer unique key
		search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
		search_snapshot << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
		
		mongo::BSONObj u = inc_update.obj();
		mongo::BSONObj q = search_snapshot.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotIncrementJobCounter insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "update:" << u;)
		DEBUG_CODE(MDBID_LDBG_ << "condition" << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotIncrementJobCounter insert ---------------------------------------------";)
		
		//update and waith until the data is on the server
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q, u, false, false, &mongo::WriteConcern::acknowledged);
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! get the dataset from a snapshot
int MongoDBDriver::snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
													const std::string& producer_unique_key,
													const std::string& dataset_type,
													void **channel_data,
													uint32_t& channel_data_size) {
	int err = 0;
	mongo::BSONObj			result;
	mongo::BSONObjBuilder	search_snapshot;
	try{
		//search for snapshot name and producer unique key
		search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name
		<< MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID << producer_unique_key;
		
		mongo::BSONObj q = search_snapshot.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotGetDatasetForProducerKey findOne ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "condition" << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotGetDatasetForProducerKey findOne ---------------------------------------------";)
		
		//update and waith until the data is on the server
		if((err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))) {
			MDBID_LERR_ << "Errore finding the snapshot "<< snapshot_name << " for the unique key "<< producer_unique_key <<"with error "<<err;
		} else {
			std::string channel_unique_key = producer_unique_key+dataset_type;
			if(result.hasField(channel_unique_key)) {
				//! get data
				mongo::BSONObj channel_data_obj = result.getObjectField(channel_unique_key);
				if((channel_data_size = channel_data_obj.objsize())) {
					*channel_data = malloc(channel_data_size);
					if(*channel_data) {
						std::memcpy(*channel_data, (void*)channel_data_obj.objdata(), channel_data_size);
					}
				}
			}
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return 0;
}

//! Delete a snapshot where no job is working
int MongoDBDriver::snapshotDeleteWithName(const std::string& snapshot_name) {
	int err = 0;
	mongo::BSONObj			result;
	mongo::BSONObjBuilder	search_snapshot;
	try{
		//search for snapshot name and producer unique key
		search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
		
		mongo::BSONObj q = search_snapshot.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotDeleteWithName count ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << "condition" << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotDeleteWithName count ---------------------------------------------";)
		
		//update and waith until the data is on the server
		if((err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q))) {
			MDBID_LERR_ << "Errore finding the snapshot "<< snapshot_name << "with error "<<err;
			return err;
		}
		
		//the snapshot to delete is present, so we delete it
		if((err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q))){
			MDBID_LERR_ << "Errore deleting the snapshot "<< snapshot_name << "with error "<<err;
			return err;
		}
		
		
		//no we need to delete all dataset associated to it
		if((err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))){
			MDBID_LERR_ << "Errore deleting the snapshot data "<< snapshot_name << "with error "<<err;
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}