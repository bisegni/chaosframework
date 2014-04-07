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

using namespace chaos::data_service::vfs;
using namespace chaos::data_service::index_system;

#define MongoDBIndexDriver_LOG_HEAD "[MongoDBIndexDriver] - "
#define MDBID_LAPP_ LAPP_ << MongoDBIndexDriver_LOG_HEAD
#define MDBID_LDBG_ LDBG_ << MongoDBIndexDriver_LOG_HEAD << __FUNCTION__ << " - "
#define MDBID_LERR_ LERR_ << MongoDBIndexDriver_LOG_HEAD << __FUNCTION__ << " - "

#define MONGO_DB_CHECK_ERROR_CODE(b) b["code"].numberInt()
#define MONGO_DB_GET_ERROR(e) \
mongo::BSONObj _error = sc->getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);

MongoDBIndexDriver::MongoDBIndexDriver(std::string alias):IndexDriver(alias) {
	
}


MongoDBIndexDriver::~MongoDBIndexDriver() {
	
}

//! init
void MongoDBIndexDriver::init(void *init_data) throw (chaos::CException) {
	IndexDriver::init(init_data);
	std::string errmsg;
	std::string servers;
	int err = 0;
	for (IndexDriverServerListIterator iter = setting->servers.begin();
		 iter != setting->servers.end();
		 iter++){
		
		servers.append(*iter);
		servers.append(",");
	}
	if(servers.size()) {
		servers.resize(servers.size()-1);
	}
	MDBID_LAPP_ << "Try to setup connection with " << servers;
	connection_string = mongo::ConnectionString::parse(servers, errmsg);
	if(!connection_string.isValid()) {
		MDBID_LDBG_ << errmsg;
		throw CException(-1, errmsg, __PRETTY_FUNCTION__);
	}
	
	//all is gone ok create the needde index
	//mongo::ScopedDbConnection sc(connection_string);
	//sc->ensureIndex(MONGO_DB_VFS_VFAT_COLLECTION, BSON("vfs_path"<< 1 << "vfs_domain" << 1), /*unique*/true);
	
	//MONGO_DB_GET_ERROR(err);
	//if(err) throw CException(-1, boost::str(boost::format("Error creating index with return code  %1%") % err),  __PRETTY_FUNCTION__);
}

//!deinit
void MongoDBIndexDriver::deinit() throw (chaos::CException) {
	IndexDriver::deinit();
}

//! Register a new data block wrote on stage area
int MongoDBIndexDriver::vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block, DataBlockState new_block_state) {
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
	mongo::ScopedDbConnection sc(connection_string);
	
	try {
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		mongo::BSONObj search_result = sc.conn().findOne(MONGO_DB_VFS_VFAT_COLLECTION, bson_search.obj());
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "Error getting file information";
			return -3;
		}
		
		//insert new
		bson_block.append(MONGO_DB_FIELD_FILE_PRIMARY_KEY, search_result["_id"].OID());
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_STATE, new_block_state);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS, (long long)data_block->creation_time);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VALID_UNTIL_TS, (long long)data_block->invalidation_timestamp);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_MAX_BLOCK_SIZE, (long long)data_block->max_reacheable_size);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH, data_block->vfs_path);
		bson_block.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		sc.conn().insert(MONGO_DB_VFS_VBLOCK_COLLECTION,  bson_block.obj());
		
		sc.done();
		
	} catch (const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -4;
	}
	return 0;
}

//! Set the state for a stage datablock
int MongoDBIndexDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block, DataBlockState state) {
	int err = 0;
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block_query;
	mongo::BSONObjBuilder bson_block_update;
	mongo::BSONObjBuilder bson_block_update_filed;
	mongo::ScopedDbConnection sc(connection_string);
	try{
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		mongo::BSONObj search_result = sc.conn().findOne(MONGO_DB_VFS_VFAT_COLLECTION, bson_search.obj());
		{
			MONGO_DB_GET_ERROR(err);
			if(err) {
				MDBID_LERR_ << "Error " << err << " searching vfs on vfat";
				return err;
			}
			if(search_result.isEmpty()) {
				//cant be here..anyway give error
				MDBID_LERR_ << "[CAN'T BE HERE]Error getting file information";
				
			}
		}
		//compose query
		bson_block_query.append(MONGO_DB_FIELD_FILE_PRIMARY_KEY, search_result["_id"].OID());
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_CREATION_TS, (long long)data_block->creation_time);
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_PATH, data_block->vfs_path);
		bson_block_query.append(MONGO_DB_FIELD_DATA_BLOCK_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		//compose udpate
		bson_block_update_filed.append(MONGO_DB_FIELD_DATA_BLOCK_STATE, state);
		bson_block_update.append("$set", bson_block_update_filed.obj());
		
		sc.conn().update(MONGO_DB_VFS_VBLOCK_COLLECTION, bson_block_query.obj(), bson_block_update.obj());
		{
			MONGO_DB_GET_ERROR(err);
			if(err) {
				MDBID_LERR_ << "Error " << err << " updating state on datablock";
			}
		}
	}catch(const mongo::DBException &e) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}

//! Heartbeat update stage block
int MongoDBIndexDriver::vfsWorkHeartBeatOnDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block) {
	return 0;
}

//! Check if the vfs file exists
int MongoDBIndexDriver::vfsFileExist(VFSFile *vfs_file, bool& exists_flag) {
	int err = 0;
	mongo::BSONObjBuilder b;
	mongo::ScopedDbConnection sc(connection_string);
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		mongo::BSONObj result = sc.conn().findOne(MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
		MONGO_DB_GET_ERROR(err);
		if(err) {
			MDBID_LERR_ << "Error " << err << " creting vfs file entry";
		} else {
			exists_flag = !result.isEmpty();
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	sc.done();
	return err;
}

//! Create a file entry into the vfat
int MongoDBIndexDriver::vfsCreateFileEntry(chaos_vfs::VFSFile *vfs_file) {
	int err = 0;
	mongo::BSONObjBuilder b;
	mongo::ScopedDbConnection sc(connection_string);
	try{
		//compose file search criteria
		b.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
		
		sc.conn().insert(MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
		
		MONGO_DB_GET_ERROR(err);
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
	sc.done();
	return err;
}