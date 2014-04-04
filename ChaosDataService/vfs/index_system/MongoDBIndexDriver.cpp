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

#define MONGO_DB_VFS_VFAT_COLLECTION	"chaos_vfs.vfat"
#define MONGO_DB_VFS_VBLOCK_COLLECTION	"chaos_vfs.datablock"

MongoDBIndexDriver::MongoDBIndexDriver(std::string alias):IndexDriver(alias) {
	
}


MongoDBIndexDriver::~MongoDBIndexDriver() {
	
}

//! init
void MongoDBIndexDriver::init(void *init_data) throw (chaos::CException) {
	IndexDriver::init(init_data);
	std::string errmsg;
	std::string servers;
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
	
	//all is gone ok
}

//!deinit
void MongoDBIndexDriver::deinit() throw (chaos::CException) {
	IndexDriver::deinit();
}

//! Register a new data block wrote on stage area
int MongoDBIndexDriver::vfsAddNewDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block) {
	int err = 0;
	bool f_exists = false;

	if(vfsFileExist(vfs_file, f_exists) || !f_exists) {
		MDBID_LERR_ << "File description not present of vfat";
		return -1;
	}
	
	//allocate data block on vfat
	mongo::BSONObjBuilder bson_search;
	mongo::BSONObjBuilder bson_block;
	mongo::ScopedDbConnection sc(connection_string);
	
	try {
		bson_search.append("v_path", vfs_file->getVFSFileInfo()->vfs_fpath);
		bson_search.append("v_domain", vfs_file->getVFSFileInfo()->vfs_domain);
		mongo::BSONObj search_result = sc.conn().findOne(MONGO_DB_VFS_VFAT_COLLECTION, bson_search.obj());
		if(search_result.isEmpty()) {
			//cant be here..anyway give error
			MDBID_LERR_ << "Error getting file information";
			return -2;
		}
		
		//insert new
		bson_block.append("file_pk", search_result["_id"].OID());
		bson_block.append("ctime", (long long)data_block->creation_time);
		bson_block.append("vfs_path", data_block->vfs_path);
		sc.conn().insert(MONGO_DB_VFS_VBLOCK_COLLECTION,  bson_block.obj());
		sc.done();

	} catch (const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -2;
	}
	return 0;
}

//! Set the state for a stage datablock
int MongoDBIndexDriver::vfsSetStateOnDataBlock(chaos_vfs::VFSFile *vfs_file, chaos_vfs::DataBlock *data_block, StageDataBlockState state) {
	return 0;
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
		b.append("v_path", vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append("v_domain", vfs_file->getVFSFileInfo()->vfs_domain);
		
		mongo::BSONObj result = sc.conn().findOne(MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
		exists_flag = !result.isEmpty();
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
		b.append("v_path", vfs_file->getVFSFileInfo()->vfs_fpath);
		b.append("v_domain", vfs_file->getVFSFileInfo()->vfs_domain);
		
		sc.conn().insert(MONGO_DB_VFS_VFAT_COLLECTION, b.obj());
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	sc.done();
	return err;
}