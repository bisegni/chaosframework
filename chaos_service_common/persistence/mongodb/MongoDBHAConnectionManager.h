/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
                    
                    inline ChaosUniquePtr<DriverScopedConnection> getConnection();
                    
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
                               const mongo::WriteConcern *wc = &mongo::WriteConcern::journaled,
                               int flags=0);
                    
                    int findOne(mongo::BSONObj& result,
                                const std::string &ns,
                                const mongo::Query& query,
                                const mongo::BSONObj *fields_to_return = 0,
                                int queryOptions = 0);
                    
                    int findN(std::vector<mongo::BSONObj>& out,
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
                               const mongo::WriteConcern *wc = &mongo::WriteConcern::journaled);
                    
                    int remove(const std::string &ns ,
                               mongo::Query q,
                               bool justOne = 0,
                               const mongo::WriteConcern *wc = &mongo::WriteConcern::journaled);
                    
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
                    
                    ChaosUniquePtr<mongo::DBClientCursor> query(const std::string &ns,
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
                    
                    int dropIndex(const std::string &ns,
                                  const std::string& index_name);
                    int getIndex(mongo::BSONObj& result,
                                 const std::string &collection,
                                 const std::string& index_name);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFrameworkShared__MongoDBHAConnectionManager__) */
