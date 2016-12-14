//
//  MongoDBDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/data/CDataWrapper.h>
#include "MongoDBDriver.h"

#include <boost/format.hpp>

#include <mongo/client/dbclient.h>

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::data_service::vfs;
using namespace chaos::data_service::db_system;

using namespace chaos::common::utility;
namespace chaos_data = chaos::common::data;

#define MDBID_LAPP_ INFO_LOG(MongoDBDriver)
#define MDBID_LDBG_ DBG_LOG(MongoDBDriver)
#define MDBID_LERR_ ERR_LOG(MongoDBDriver)

DEFINE_CLASS_FACTORY(MongoDBDriver, DBDriver);
//!
MongoDBDriver::MongoDBDriver(std::string alias):
DBDriver(alias),
ha_connection_pool(NULL){}

//!
MongoDBDriver::~MongoDBDriver() {}

//! init
void MongoDBDriver::init(void *init_data) throw (chaos::CException) {
	DBDriver::init(init_data);
	std::string errmsg;
	std::string servers;
	//allcoate ha pool class
	ha_connection_pool = new service_common::persistence::mongodb::MongoDBHAConnectionManager(setting->servers, setting->key_value_custom_param);
	if(setting->key_value_custom_param.count("db")) {
		db_name = setting->key_value_custom_param["db"];
	} else {
		db_name = MONGO_DB_NAME;
	}
}

//!deinit
void MongoDBDriver::deinit() throw (chaos::CException) {
	DBDriver::deinit();
    if(ha_connection_pool) {delete(ha_connection_pool);}
}

//! Create a new snapshot
int MongoDBDriver::snapshotCreateNewWithName(const std::string& snapshot_name,
											 std::string& working_job_unique_id) {
	int err = 0;
	mongo::BSONObjBuilder	new_snapshot_start;
	
	//----- generate the random code ------ for this snapshot
	working_job_unique_id = UUIDUtil::generateUUIDLite();
	try{
        uint64_t ts = TimingUtil::getTimeStamp();
		new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_NAME << snapshot_name;
		new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_TS_DATE << mongo::Date_t(ts);
        new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_TS << (long long)ts;
		new_snapshot_start << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
		
		mongo::BSONObj q = new_snapshot_start.obj();
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		DEBUG_CODE(MDBID_LDBG_ << q;)
		DEBUG_CODE(MDBID_LDBG_ << "snapshotCreateNewWithName insert ---------------------------------------------";)
		
		err = ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q);
		if(err == 11000) {
			//already exis a snapshot with that name so no error need to be throw
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
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q, u, true, false);
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
		err = ha_connection_pool->update(MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q, u, false, false);
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
		if((err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))) {
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
		if((err = ha_connection_pool->findOne(result, MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q))) {
			MDBID_LERR_ << "Errore finding the snapshot "<< snapshot_name << "with error "<<err;
			return err;
		}
		
		//the snapshot to delete is present, so we delete it
		if((err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT), q))){
			MDBID_LERR_ << "Errore deleting the snapshot "<< snapshot_name << "with error "<<err;
			return err;
		}
		
		
		//no we need to delete all dataset associated to it
		if((err = ha_connection_pool->remove(MONGO_DB_COLLECTION_NAME_COL(db_name, MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))){
			MDBID_LERR_ << "Errore deleting the snapshot data "<< snapshot_name << "with error "<<err;
		}
	} catch( const mongo::DBException &e ) {
		MDBID_LERR_ << e.what();
		err = -1;
	}
	return err;
}
