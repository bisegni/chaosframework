/*
 *	MongoDBHAConnection.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__MongoDBHAConnection__
#define __CHAOSFramework__MongoDBHAConnection__

#include "mongo/client/dbclient.h"

#include <vector>
#include <string>
#include <queue>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace data_service {
		namespace index_system {
			
			/*!
			 Class that encapsulat ethe mongodb conenction for safe deallocation
			 */
			class DriverScopedConnection : public mongo::ScopedDbConnection {
				
			public:
				DriverScopedConnection(mongo::ConnectionString _conn);
				~DriverScopedConnection();
			};
			
			typedef DriverScopedConnection *MongoDBHAConnection;
			
			/*!
			 Implementation for the high availability with multiple
			 mongos router instance
			 */
			class MongoDBHAConnectionManager {
				uint32_t server_number;
				
				uint64_t next_retrive_intervall;
				
				boost::shared_mutex mutext_queue;
				std::queue< boost::shared_ptr<mongo::ConnectionString> > valid_connection_queue;
				std::queue< boost::shared_ptr<mongo::ConnectionString> > offline_connection_queue;
			
				inline bool canRetry();
			public:
				MongoDBHAConnectionManager(std::vector<std::string> monogs_routers_list);
				~MongoDBHAConnectionManager();
				bool getConnection(MongoDBHAConnection *connection_sptr);
				
				int insert( const std::string &ns , mongo::BSONObj obj , int flags=0);
				int findOne( mongo::BSONObj& result, const std::string &ns, const mongo::Query& query, const mongo::BSONObj *fieldsToReturn = 0, int queryOptions = 0);
				int update( const std::string &ns, mongo::Query query, mongo::BSONObj obj, bool upsert = false, bool multi = false );
				mongo::BSONObj getLastError();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBHAConnection__) */
