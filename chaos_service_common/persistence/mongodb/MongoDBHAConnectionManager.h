/*
 *	MongoDBHAConnection.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFrameworkShared__MongoDBHAConnectionManager__
#define __CHAOSFrameworkShared__MongoDBHAConnectionManager__

#include <mongo/client/dbclient.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/network/URLServiceFeeder.h>

#include <vector>
#include <string>
#include <queue>
#include <map>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace mongodb {
                
                struct ConnectionInfo {
                    int service_index;
                    std::string user;
                    std::string password;
                    std::string db;
                    mongo::ConnectionString conn_str;
                };
                
                /*!
                 Class that encapsulat ethe mongodb conenction for safe deallocation
                 */
                class DriverScopedConnection:
                public mongo::ScopedDbConnection  {
                public:
                    int service_index;
                    DriverScopedConnection(mongo::ConnectionString& conn_str,
                                           double timeout);
                    ~DriverScopedConnection();
                };
                
                typedef DriverScopedConnection *MongoDBHAConnection;
                
                struct MongoAuthHook :
                public mongo::DBConnectionHook {
                    std::string user;
                    std::string pwd;
                    std::string db;
                    bool has_autentication;
                    void onCreate( mongo::DBClientBase * conn );
                    void onHandedOut( mongo::DBClientBase * conn );
                    void onDestroy( mongo::DBClientBase * conn );
                public:
                    MongoAuthHook(std::map<std::string,std::string>& key_value_custom_param);
                    const std::string& getDatabaseName();
                    const std::string& getDatabaseUsername();
                };
                
                /*!
                 Implementation for the high availability with multiple
                 mongos router instance
                 */
                class MongoDBHAConnectionManager:
                public common::network::URLServiceFeederHandler {
                    MongoAuthHook *hook;
                    
                    common::network::URLServiceFeeder service_feeder;
                    std::queue< uint32_t > offline_connection_queue;
                    uint32_t server_number;
                    
                    uint64_t next_retrive_intervall;
                    
                    inline bool canRetry();
                    
                    inline std::auto_ptr<DriverScopedConnection> getConnection();
                    
                    inline bool isConnectionError(int error);
                    
                    //inherithed by service feeder
                    void disposeService(void *service_ptr);
                    
                    void* serviceForURL(const common::network::URL& url,
                                        uint32_t service_index);
                public:
                    MongoDBHAConnectionManager(ChaosStringVector monogs_routers_list,
                                               std::map<std::string, std::string>& key_value_custom_param);
                    ~MongoDBHAConnectionManager();
                    const std::string& getDatabaseName();
                    
                    int insert(const std::string &ns,
                               mongo::BSONObj obj,
                               const mongo::WriteConcern& wc = mongo::WriteConcern::journaled,
                               int flags=0);
                    
                    int findOne(mongo::BSONObj& result,
                                const std::string &ns,
                                const mongo::Query& query,
                                const mongo::BSONObj *fields_to_return = 0,
                                int queryOptions = 0);
                    
                    void findN(std::vector<mongo::BSONObj>& out,
                               const std::string& ns,
                               mongo::Query query,
                               int nToReturn,
                               int nToSkip = 0,
                               const mongo::BSONObj *fields_to_return = 0,
                               int queryOptions = 0);
                    
                    int findAndModify(mongo::BSONObj& result,
                                      const std::string& ns,
                                      const mongo::BSONObj& query,
                                      const mongo::BSONObj& update,
                                      bool upsert = false,
                                      bool returnNew = false,
                                      const mongo::BSONObj& sort = mongo::BSONObj(),
                                      const mongo::BSONObj& fields = mongo::BSONObj());
                    
                    int findAndRemove(mongo::BSONObj& result,
                                      const std::string& ns,
                                      const mongo::BSONObj& query,
                                      const mongo::BSONObj& sort = mongo::BSONObj(),
                                      const mongo::BSONObj& fields = mongo::BSONObj());
                    
                    int runCommand(mongo::BSONObj& result,
                                   const std::string &ns,
                                   const mongo::BSONObj& comand,
                                   int queryOptions = 0);
                    
                    int update(const std::string &ns,
                               mongo::Query query,
                               mongo::BSONObj obj,
                               bool upsert = false,
                               bool multi = false,
                               const mongo::WriteConcern& wc = mongo::WriteConcern::journaled);
                    
                    int remove(const std::string &ns ,
                               mongo::Query q,
                               bool justOne = 0,
                               const mongo::WriteConcern& wc = mongo::WriteConcern::journaled);
                    
                    mongo::BSONObj distinct(const std::string &ns,
                                            const std::string &field,
                                            const mongo::Query& query = mongo::Query());
                    
                    int ensureIndex(const std::string &database,
                                    const std::string &collection,
                                    mongo::BSONObj keys,
                                    bool unique = false,
                                    const std::string &name = "",
                                    bool dropDup = false,
                                    bool background = false,
                                    int v = -1,
                                    int ttl = 0 );
                    
                    std::auto_ptr<mongo::DBClientCursor> query(const std::string &ns,
                                                               mongo::Query query,
                                                               int nToReturn = 0,
                                                               int nToSkip = 0,
                                                               const mongo::BSONObj *fieldsToReturn = 0,
                                                               int queryOptions = 0,
                                                               int batchSize = 0 );
                    
                    int count(unsigned long long & result,
                              const std::string &ns,
                              const mongo::Query& query = mongo::Query(),
                              int options=0,
                              int limit=0,
                              int skip=0);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFrameworkShared__MongoDBHAConnectionManager__) */
