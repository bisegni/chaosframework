//
//  MongoDBHAConnection.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/04/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>

#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>

#define RETRIVE_MIN_TIME 500
#define RETRIVE_MAX_TIME 10000

#define MongoDBHAConnection_LOG_HEAD "[MongoDBHAConnection] - "
#define MDBHAC_LAPP_ LAPP_ << MongoDBHAConnection_LOG_HEAD
#define MDBHAC_LDBG_ LDBG_ << MongoDBHAConnection_LOG_HEAD << __FUNCTION__ << " - "
#define MDBHAC_LERR_ LERR_ << MongoDBHAConnection_LOG_HEAD << __FUNCTION__ << " - "

#define MONGO_DB_CHECK_ERROR_CODE(b) b["code"].numberInt()
#define MONGO_DB_CHECK_ERROR_MESSAGE(b) b["errmsg"].valuestrsafe()

#define MONGO_DB_GET_ERROR(c, e) \
mongo::BSONObj _error = c->conn().getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);

#define MONGO_DB_GET_ERROR_WHIT_MESSAGE(c, e, m) \
mongo::BSONObj _error = c->conn().getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);\
m = MONGO_DB_CHECK_ERROR_MESSAGE(_error);

#define CONTINUE_ON_NEXT_CONNECTION(x) \
switch(x) { \
case 13328: \
case 10276: \
case 13386: \
case 16090: \
case 13127: \
case 13348: \
case 13678: \
	DEBUG_CODE(MDBHAC_LERR_ << "Conenction error:" << x;)\
	continue; \
break; \
default: \
break; \
}





using namespace chaos::service_common::persistence::mongodb;
//-----------------------------------------------------------------------------------------------------------

MongoAuthHook::MongoAuthHook(std::map<std::string,std::string>& key_value_custom_param):
has_autentication(false) {
    if(key_value_custom_param.count("user")) {
        user = key_value_custom_param["user"];
    }
    
    if(key_value_custom_param.count("pwd")){
        pwd = key_value_custom_param["pwd"];
    }
	   
    if(key_value_custom_param.count("db")) {
        db = key_value_custom_param["db"];
    }
		
		
    has_autentication = key_value_custom_param.count("user") || key_value_custom_param.count("pwd");
}

void MongoAuthHook::onCreate( mongo::DBClientBase * conn ) {
	std::string err;
	if(has_autentication){
		MDBHAC_LDBG_ << "Autenticate on - " << conn->getServerAddress();
		if(!conn->auth(db, user, pwd, err)) {
			MDBHAC_LERR_ << conn->getServerAddress() << " -> " << err << " user:" << user << " pwd:" <<pwd;
		}
	} else {
		MDBHAC_LDBG_ << "No Autenticate on - " << conn->getServerAddress();
	}
}

void MongoAuthHook::onHandedOut( mongo::DBClientBase * conn ) {
	//MDBHAC_LAPP_ << "MongoDBHAConnectionManager::onHandedOut - " << conn->getServerAddress();
}

void MongoAuthHook::onDestroy( mongo::DBClientBase * conn ) {
	MDBHAC_LAPP_ << "MongoDBHAConnectionManager::onDestroy - " << conn->getServerAddress();
}

const std::string& MongoAuthHook::getDatabaseName() {
    return db;
}

const std::string& MongoAuthHook::getDatabaseUsername() {
    return user;
}
//-----------------------------------------------------------------------------------
DriverScopedConnection::DriverScopedConnection(mongo::ConnectionString _conn):
ScopedDbConnection(_conn) {}

DriverScopedConnection::DriverScopedConnection(mongo::ConnectionString _conn,
											   double timeout):
ScopedDbConnection(_conn, timeout) {}

DriverScopedConnection::~DriverScopedConnection() {
	ScopedDbConnection::done();
}


//-----------------------------------------------------------------------------------------------------------

MongoDBHAConnectionManager::MongoDBHAConnectionManager(std::vector<std::string> monogs_routers_list,
													   std::map<std::string,std::string>& key_value_custom_param):
server_number((uint32_t)monogs_routers_list.size()),
next_retrive_intervall(0),
hook(NULL){
	std::string errmsg;
	std::string complete_url;
	for (std::vector<std::string>::iterator iter = monogs_routers_list.begin();
		 iter != monogs_routers_list.end();
		 iter++){
		complete_url = boost::str(boost::format("%1%") % *iter);
		MDBHAC_LAPP_ << "Register mongo server address " << complete_url;
		boost::shared_ptr<mongo::ConnectionString> cs_ptr(new mongo::ConnectionString(mongo::HostAndPort(complete_url)));
		valid_connection_queue.push(cs_ptr);
	}
	
	mongo::pool.addHook(hook = new MongoAuthHook(key_value_custom_param));
	
	mongo::Status status = mongo::client::initialize();
	if (!status.isOK()) {
	  MDBHAC_LERR_ << "failed to initialize the client driver: " << status.toString();
    }
}

MongoDBHAConnectionManager::~MongoDBHAConnectionManager() {
    if(hook) delete(hook);
	std::queue< boost::shared_ptr<mongo::ConnectionString> > empty_queue;
	std::swap(valid_connection_queue, empty_queue);
	std::swap(offline_connection_queue, empty_queue);
	
}

const std::string& MongoDBHAConnectionManager::getDatabaseName() {
    return hook->getDatabaseName();
}

inline bool MongoDBHAConnectionManager::canRetry() {
	bool retry = false;
	uint64_t cur_ts = common::utility::TimingUtil::getTimeStamp();
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
	
	DEBUG_CODE(MDBHAC_LDBG_ << "Try " << valid_server_num << " connections";)
	
	//try fo find a good conncetion
	while(!nextCS && cur_index < valid_server_num) {
		cur_index++;
		DEBUG_CODE(MDBHAC_LDBG_ << "Try server " << cur_index;)

		// get next available server connection string
		if((nextCS = valid_connection_queue.front())) {
			//remove invalid connection string form queue and put it into the offline one
			valid_connection_queue.pop();
			try {
				DriverScopedConnection c(*nextCS, 1);
				connection_is_good = c.ok();
				DEBUG_CODE(MDBHAC_LDBG_ << "(idx:" << cur_index << ") connection_is_good:" << connection_is_good;)
				if(connection_is_good) {
					DEBUG_CODE(MDBHAC_LDBG_ << "(idx:" << cur_index << ") set default write concern";)
					c.get()->setWriteConcern(mongo::WriteConcern::journaled);
				}
			} catch (std::exception &ex) {
				DEBUG_CODE(MDBHAC_LERR_ << "(idx:" << cur_index << ") exception:" << ex.what();)

				// in any case of error put the current conneciton string into offline queue
				offline_connection_queue.push(nextCS);
				//check nex string if there is one
				nextCS.reset();
				continue;
			}
			if(connection_is_good) {
				DEBUG_CODE(MDBHAC_LDBG_ << "(idx:" << cur_index << ") connection is good";)
				//put the used description at the end of the queue
				valid_connection_queue.push(nextCS);
			} else {
				DEBUG_CODE(MDBHAC_LDBG_ << "(idx:" << cur_index << ") connection is NOT good";)

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

int MongoDBHAConnectionManager::insert( const std::string &ns,
									   mongo::BSONObj obj,
									   int flags) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().insert(ns, obj, flags);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::findOne(mongo::BSONObj& result,
										const std::string &ns,
										const mongo::Query& query,
										const mongo::BSONObj *fields_to_return,
										int queryOptions) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			result = conn->conn().findOne(ns, query, fields_to_return, queryOptions);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}

void MongoDBHAConnectionManager::findN(std::vector<mongo::BSONObj>& out,
									   const std::string& ns,
									   mongo::Query query,
									   int nToReturn,
									   int nToSkip,
									   const mongo::BSONObj *fields_to_return,
									   int queryOptions) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().findN(out, ns, query, nToReturn, nToSkip, fields_to_return, queryOptions);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
}

std::auto_ptr<mongo::DBClientCursor> MongoDBHAConnectionManager::query(const std::string &ns,
																	   mongo::Query query,
																	   int nToReturn,
																	   int nToSkip,
																	   const mongo::BSONObj *fieldsToReturn,
																	   int queryOptions,
																	   int batchSize) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	std::auto_ptr<mongo::DBClientCursor> result;
	while (getConnection(&conn)) {
		try {
			result = conn->conn().query(ns, query, nToReturn, nToSkip, fieldsToReturn, queryOptions, batchSize);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return result;
}


int MongoDBHAConnectionManager::runCommand(mongo::BSONObj& result,
										   const std::string &ns,
										   const mongo::BSONObj& command,
										   int queryOptions) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			if(!conn->conn().runCommand(ns, command, result, queryOptions)) {
				MDBHAC_LERR_ << "Error executing MongoDBHAConnectionManager::runCommand" << " -> " << command.toString();
			}
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::runCommand" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::update( const std::string &ns,
									   mongo::Query query,
									   mongo::BSONObj obj,
									   bool upsert,
									   bool multi,
									   const mongo::WriteConcern* wc) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().update(ns, query, obj, upsert, multi, wc);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::findAndModify(mongo::BSONObj& result,
                                              const std::string& ns,
                                              const mongo::BSONObj& query,
                                              const mongo::BSONObj& update,
                                              bool upsert,
                                              bool returnNew,
                                              const mongo::BSONObj& sort,
                                              const mongo::BSONObj& fields) {
    int err = -1;
    MongoDBHAConnection conn = NULL;
    while (getConnection(&conn)) {
        try {
            result = conn->conn().findAndModify(ns,
                                                query,
                                                update,
                                                upsert,
                                                returnNew,
                                                sort,
                                                fields);
            MONGO_DB_GET_ERROR(conn, err);
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findAndModify" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            DELETE_OBJ_POINTER(conn)
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
        break;
    }
    if(conn) delete(conn);
    return err;
}


int MongoDBHAConnectionManager::findAndRemove(mongo::BSONObj& result,
                                              const std::string& ns,
                                              const mongo::BSONObj& query,
                                              const mongo::BSONObj& sort,
                                              const mongo::BSONObj& fields) {
    int err = -1;
    MongoDBHAConnection conn = NULL;
    while (getConnection(&conn)) {
        try {
            result = conn->conn().findAndRemove(ns,
                                                query,
                                                sort,
                                                fields);
            MONGO_DB_GET_ERROR(conn, err);
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findAndRemove" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            DELETE_OBJ_POINTER(conn)
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
        break;
    }
    if(conn) delete(conn);
    return err;
}

int MongoDBHAConnectionManager::remove( const std::string &ns , mongo::Query q , bool justOne, const mongo::WriteConcern* wc) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			conn->conn().remove(ns, q, justOne, wc);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::remove" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}


int MongoDBHAConnectionManager::count(unsigned long long & result,
									  const std::string &ns,
									  const mongo::Query& query,
									  int options,
									  int limit,
									  int skip) {
	int err = 0;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			result = conn->conn().count(ns, query, options, limit, skip);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}

int MongoDBHAConnectionManager::ensureIndex( const std::string &database,
											const std::string &collection,
											mongo::BSONObj keys,
											bool unique,
											const std::string &name,
											bool dropDup,
											bool background,
											int v,
											int ttl) {
	int err = -1;
	MongoDBHAConnection conn = NULL;
	while (getConnection(&conn)) {
		try {
			mongo::BSONObjBuilder toSave;
			toSave.append( "ns" , database+"."+collection );
			toSave.append( "key" , keys );
			
			if ( name != "" ) {
				toSave.append( "name" , name );
			}
			else {
				std::string nn = conn->conn().genIndexName( keys );
				toSave.append( "name" , nn );
			}
			
			if( v >= 0 )
				toSave.append("v", v);
			
			if ( unique )
				toSave.appendBool( "unique", unique );
			
			if( background )
				toSave.appendBool( "background", true );
			
			if( dropDup )
				toSave.appendBool( "dropDups", dropDup );
			
			
			if ( ttl > 0 )
				toSave.append( "expireAfterSeconds", ttl );
		
			err = insert(database+".system.indexes", toSave.obj());
			//err = conn->conn().ensureIndex(database+"."+collection, keys, unique, name, cache, background, v, ttl);
			MONGO_DB_GET_ERROR(conn, err);
		} catch (std::exception& ex) {
			MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
			MONGO_DB_GET_ERROR(conn, err);
			DELETE_OBJ_POINTER(conn)
			CONTINUE_ON_NEXT_CONNECTION(err)
		}
		break;
	}
	if(conn) delete(conn);
	return err;
}
