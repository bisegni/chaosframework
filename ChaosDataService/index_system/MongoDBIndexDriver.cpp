//
//  MongoDBIndexDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MongoDBIndexDriver.h"

#include <mongo/client/dbclient.h>
using namespace chaos::data_service::index_system;

MongoDBIndexDriver::MongoDBIndexDriver(std::string alias):IndexDriver(alias) {
	
}


MongoDBIndexDriver::~MongoDBIndexDriver() {
	
}

//! init
void MongoDBIndexDriver::init(void *init_data) throw (chaos::CException) {
	IndexDriver::init(init_data);
	std::string errmsg;
	for (IndexDriverServerListIterator iter = setting->servers.begin();
		 iter != setting->servers.end();
		 iter++){
		
		mongo::ConnectionString cs = mongo::ConnectionString::parse(*iter, errmsg);
		if(!cs.isValid()) {
			throw chaos::CException(-1, errmsg, __PRETTY_FUNCTION__);
		}
		
		
	}
	
}

//!deinit
void MongoDBIndexDriver::deinit() throw (chaos::CException) {
	IndexDriver::deinit();
}

//! Register a new data block wrote on stage area
int MongoDBIndexDriver::addNewStageDataBlock(chaos_vfs::DataBlock *data_block) {
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