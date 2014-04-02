//
//  MongoDBIndexDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MongoDBIndexDriver.h"

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

using namespace chaos::data_service::index_system;

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
int MongoDBIndexDriver::addNewStageDataBlock(chaos_vfs::DataBlock *data_block) {
	mongo::ScopedDbConnection sc(connection_string);
	mongo::BSONObjBuilder b;
	b.append("name", "Joe");
	b.append("age", 33);
	
	sc.conn().insert("chaos_index.stage_block",  b.obj());
	sc.done();
	return 0;
}

//! Set the state for a stage datablock
int MongoDBIndexDriver::setStateOnStageDataBlock(chaos_vfs::DataBlock *data_block, StageDataBlockState state) {
	return 0;
}

//! Heartbeat update stage block
int MongoDBIndexDriver::workHeartBeatOnStageDataBlock(chaos_vfs::DataBlock *data_block) {
	return 0;
}

//! Retrive the path for all datablock in a determinate state
int MongoDBIndexDriver::getStageDataBlockPathByState(std::vector<std::string>& path_data_block, StageDataBlockState state) {
	return 0;
}