//
//  MongoDBHAConnection.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/04/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MongoDBHAConnectionManager.h"

#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>

#define RETRIVE_MIN_TIME 500
#define RETRIVE_MAX_TIME 1500

#define MongoDBHAConnection_LOG_HEAD "[MongoDBHAConnection] - "
#define MDBHAC_LAPP_ LAPP_ << MongoDBHAConnection_LOG_HEAD
#define MDBHAC_LDBG_ LDBG_ << MongoDBHAConnection_LOG_HEAD << __FUNCTION__ << " - "
#define MDBHAC_LERR_ LERR_ << MongoDBHAConnection_LOG_HEAD << __FUNCTION__ << " - "

#define MONGO_DB_CHECK_ERROR_CODE(b) b["code"].numberInt()
#define MONGO_DB_GET_ERROR(c, e) \
mongo::BSONObj _error = c->conn().getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);

using namespace chaos::data_service::index_system;

DriverScopedConnection::DriverScopedConnection(mongo::ConnectionString _conn):
ScopedDbConnection(_conn) {}

DriverScopedConnection::~DriverScopedConnection() {
	ScopedDbConnection::done();
}
//-----------------------------------------------------------------------------------------------------------

MongoDBHAConnectionManager::MongoDBHAConnectionManager(std::vector<std::string> monogs_routers_list):
server_number((uint32_t)monogs_routers_list.size()),
next_retrive_intervall(0){
	
	std::string errmsg;
	std::string complete_url;
	for (std::vector<std::string>::iterator iter = monogs_routers_list.begin();
		 iter != monogs_routers_list.end();
		 iter++){
		complete_url = boost::str(boost::format("%1%/?w=2&wtimeoutMS=2000") % *iter);
		MDBHAC_LAPP_ << "Register mongo server address " << complete_url;
		boost::shared_ptr<mongo::ConnectionString> cs_ptr(new mongo::ConnectionString(complete_url));
		valid_connection_queue.push(cs_ptr);
	}
}

MongoDBHAConnectionManager::~MongoDBHAConnectionManager() {
	std::queue< boost::shared_ptr<mongo::ConnectionString> > empty_queue;
	std::swap(valid_connection_queue, empty_queue);
	std::swap(offline_connection_queue, empty_queue);
	
}

inline bool MongoDBHAConnectionManager::canRetry() {
	bool retry = false;
	uint64_t cur_ts = chaos::TimingUtil::getTimeStamp();
	if((retry = cur_ts > next_retrive_intervall)) {
		next_retrive_intervall = cur_ts + RETRIVE_MAX_TIME;
	}
	return retry;
}

bool MongoDBHAConnectionManager::getConnection(MongoDBHAConnection *connection_sptr) {
	uint32_t cur_index = 0;
	bool connection_is_good = false;
	bool result = false;
	boost::shared_ptr<mongo::ConnectionString> nextCS;
	
	//lock the mutex for access to the queue
	boost::unique_lock<boost::shared_mutex> lock(mutext_queue);
	
	if(canRetry() && !offline_connection_queue.empty()) {
		// ad an invalid conneciton string to the end of valid queue
		valid_connection_queue.push(offline_connection_queue.front());
		
		//remove invalid connection string to his queue
		offline_connection_queue.pop();
	}

	//get the number of valid server
	uint32_t valid_server_num = (uint32_t)valid_connection_queue.size();

	//try fo find a good conncetion
	while(!nextCS && cur_index < valid_server_num) {
		cur_index++;
		// get next available server connection string
		if((nextCS = valid_connection_queue.front())) {
			//remove invalid connection string form queue and put it into the offline one
			valid_connection_queue.pop();
			try {
				DriverScopedConnection c(*nextCS);
				connection_is_good = c.ok();
			} catch (std::exception &ex) {
				// in any case of error put the current conneciton string into offline queue
				offline_connection_queue.push(nextCS);
				//check nex string if there is one
				nextCS.reset();
				continue;
			}
			if(connection_is_good) {
				//put the used description at the end of the queue
				valid_connection_queue.push(nextCS);
			} else {
				// push into offline queue
				offline_connection_queue.push(nextCS);
				
				//check nex string if there is one
				nextCS.reset();
				continue;
			}
		}
	}
	if((result = (nextCS.get() != NULL))) {
		//i can create the connection
		*connection_sptr = new DriverScopedConnection(*nextCS.get());
	}
	return result;
}

int MongoDBHAConnectionManager::insert( const std::string &ns , mongo::BSONObj obj , int flags) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().insert(ns, obj, flags);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			if(conn) delete(conn);
			continue;
		}
		err = 0;
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::findOne(mongo::BSONObj& result, const std::string &ns, const mongo::Query& query, const mongo::BSONObj *fieldsToReturn, int queryOptions) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			result = conn->conn().findOne(ns, query, fieldsToReturn, queryOptions);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			if(conn) delete(conn);
			continue;
		}
		err = 0;
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::update( const std::string &ns, mongo::Query query, mongo::BSONObj obj, bool upsert, bool multi) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().update(ns, query, obj, upsert, multi);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			if(conn) delete(conn);
			continue;
		}
		err = 0;
		break;
	}
	if(conn) delete(conn);
	return err;
}

mongo::BSONObj getLastError() {
	
}