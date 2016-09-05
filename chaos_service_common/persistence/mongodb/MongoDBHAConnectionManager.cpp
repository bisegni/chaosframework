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
mongo::BSONObj _error = c->getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);

#define MONGO_DB_GET_ERROR_WHIT_MESSAGE(c, e, m) \
mongo::BSONObj _error = c->getLastErrorDetailed(); \
e = MONGO_DB_CHECK_ERROR_CODE(_error);\
m = MONGO_DB_CHECK_ERROR_MESSAGE(_error);

#define CONTINUE_ON_NEXT_CONNECTION(x) \
switch(x) { \
case 11005: \
case 13328: \
case 10276: \
case 13386: \
case 16090: \
case 13127: \
case 13348: \
case 13678: \
service_feeder.setURLOffline(conn->service_index);\
DEBUG_CODE(MDBHAC_LERR_ << "Connection error:" << x;)\
continue; \
break; \
default: \
break; \
}




using namespace chaos::common::network;
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
DriverScopedConnection::DriverScopedConnection():
DBClientConnection (true) {}

DriverScopedConnection::~DriverScopedConnection() {}


//-----------------------------------------------------------------------------------------------------------

MongoDBHAConnectionManager::MongoDBHAConnectionManager(ChaosStringVector monogs_routers_list,
                                                       std::map<std::string,std::string>& key_value_custom_param):
service_feeder("MongoDBHAConnectionManager", this),
server_number((uint32_t)monogs_routers_list.size()),
next_retrive_intervall(0),
hook(NULL){
    std::string errmsg;
    std::string complete_url;
    hook = new MongoAuthHook(key_value_custom_param);
    for (std::vector<std::string>::iterator iter = monogs_routers_list.begin();
         iter != monogs_routers_list.end();
         iter++){
        service_feeder.addURL(*iter);
        MDBHAC_LAPP_ << "Register mongo server address " << *iter;
    }
    
    try {
        mongo::Status status = mongo::client::initialize();
        if (!status.isOK()) {
            MDBHAC_LERR_ << "failed to initialize the client driver: " << status.toString();
        }
    } catch(const mongo::DBException& xcp) {
        MDBHAC_LERR_ << xcp.what();
    } catch(...) {
        MDBHAC_LERR_ << "Undefined error";
    }
}

MongoDBHAConnectionManager::~MongoDBHAConnectionManager() {
    if(hook) delete(hook);
}

void MongoDBHAConnectionManager::disposeService(void *service_ptr) {
    if(service_ptr) delete((DriverScopedConnection*) service_ptr);
}

void* MongoDBHAConnectionManager::serviceForURL(const URL& url,
                                                uint32_t service_index) {
    std::string msg;
    DriverScopedConnection *conn = new DriverScopedConnection();
    conn->connect(mongo::HostAndPort(url.getURL()), msg);
    conn->auth(hook->db, hook->user, hook->pwd, msg);
    conn->service_index = service_index;
    return (void*)conn;
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

DriverScopedConnection* MongoDBHAConnectionManager::getConnection() {
    //lock the mutex for access to the queue
    if(canRetry() && !offline_connection_queue.empty()) {
        // ad an invalid conneciton string to the end of valid queue
        service_feeder.setURLOnline(offline_connection_queue.front());
        
        //remove invalid connection string to his queue
        offline_connection_queue.pop();
    }
    
    return static_cast<DriverScopedConnection*>(service_feeder.getService());
}

int MongoDBHAConnectionManager::fastInsert( const std::string &ns,
                                           mongo::BSONObj obj,
                                           mongo::WriteConcern wc,
                                           int flags) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            conn->insert(ns, obj, flags, &wc);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    //if(conn) delete(conn);
    return err;
}

int MongoDBHAConnectionManager::insert( const std::string &ns,
                                       mongo::BSONObj obj,
                                       mongo::WriteConcern wc,
                                       int flags) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            conn->insert(ns, obj, flags, &wc);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::insert" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    //if(conn) delete(conn);
    return err;
}

int MongoDBHAConnectionManager::findOne(mongo::BSONObj& result,
                                        const std::string &ns,
                                        const mongo::Query& query,
                                        const mongo::BSONObj *fields_to_return,
                                        int queryOptions) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->findOne(ns, query, fields_to_return, queryOptions);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}

void MongoDBHAConnectionManager::findN(std::vector<mongo::BSONObj>& out,
                                       const std::string& ns,
                                       mongo::Query query,
                                       int nToReturn,
                                       int nToSkip,
                                       const mongo::BSONObj *fields_to_return,
                                       int queryOptions) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            conn->findN(out, ns, query, nToReturn, nToSkip, fields_to_return, queryOptions);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
}

std::auto_ptr<mongo::DBClientCursor> MongoDBHAConnectionManager::query(const std::string &ns,
                                                                       mongo::Query query,
                                                                       int nToReturn,
                                                                       int nToSkip,
                                                                       const mongo::BSONObj *fieldsToReturn,
                                                                       int queryOptions,
                                                                       int batchSize) {
    int err = 0;
    std::auto_ptr<mongo::DBClientCursor> result;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->query(ns, query, nToReturn, nToSkip, fieldsToReturn, queryOptions, batchSize);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return result;
}


int MongoDBHAConnectionManager::runCommand(mongo::BSONObj& result,
                                           const std::string &ns,
                                           const mongo::BSONObj& command,
                                           int queryOptions) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            if(!conn->runCommand(ns, command, result, queryOptions)) {
                MDBHAC_LERR_ << "Error executing MongoDBHAConnectionManager::runCommand" << " -> " << command.toString();
            }
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}

int MongoDBHAConnectionManager::update( const std::string &ns,
                                       mongo::Query query,
                                       mongo::BSONObj obj,
                                       bool upsert,
                                       bool multi,
                                       const mongo::WriteConcern* wc) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            conn->update(ns, query, obj, upsert, multi, wc);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
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
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->findAndModify(ns,
                                         query,
                                         update,
                                         upsert,
                                         returnNew,
                                         sort,
                                         fields);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}


int MongoDBHAConnectionManager::findAndRemove(mongo::BSONObj& result,
                                              const std::string& ns,
                                              const mongo::BSONObj& query,
                                              const mongo::BSONObj& sort,
                                              const mongo::BSONObj& fields) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->findAndRemove(ns,
                                         query,
                                         sort,
                                         fields);
            
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}

int MongoDBHAConnectionManager::remove( const std::string &ns , mongo::Query q , bool justOne, const mongo::WriteConcern* wc) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            conn->remove(ns, q, justOne, wc);
            
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}

mongo::BSONObj MongoDBHAConnectionManager::distinct(const std::string &ns,
                                                    const std::string &field,
                                                    const mongo::Query& query) {
    int err = 0;
    mongo::BSONObj result;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->distinct(ns,
                                    field,
                                    query);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return result;
}

int MongoDBHAConnectionManager::count(unsigned long long & result,
                                      const std::string &ns,
                                      const mongo::Query& query,
                                      int options,
                                      int limit,
                                      int skip) {
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            result = conn->count(ns, query, options, limit, skip);
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
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
    int err = 0;
    DriverScopedConnection *conn = NULL;
    while((conn = getConnection())) {
        try {
            mongo::BSONObjBuilder toSave;
            toSave.append( "ns" , database+"."+collection );
            toSave.append( "key" , keys );
            
            if ( name != "" ) {
                toSave.append( "name" , name );
            }
            else {
                std::string nn = conn->genIndexName( keys );
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
            MONGO_DB_GET_ERROR(conn, err);
            break;
        } catch (std::exception& ex) {
            MDBHAC_LERR_ << "MongoDBHAConnectionManager::findOne" << " -> " << ex.what();
            MONGO_DB_GET_ERROR(conn, err);
            CONTINUE_ON_NEXT_CONNECTION(err)
        }
    }
    return err;
}
