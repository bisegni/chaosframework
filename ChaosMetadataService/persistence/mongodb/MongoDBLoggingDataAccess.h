/*
 *	MongoDBLoggingDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 11/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__MongoDBLoggingDataAccess_h
#define __CHAOSFramework__MongoDBLoggingDataAccess_h

#include "../data_access/LoggingDataAccess.h"
#include "MongoDBUtilityDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for manage the log
                /*!
                 */
                class MongoDBLoggingDataAccess:
                public data_access::LoggingDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBUtilityDataAccess *utility_data_access = NULL;
                    //return the query for a page
                    mongo::Query getNextPagedQuery(uint64_t last_sequence_before_this_page,
                                                   const std::string& source_uid,
                                                   const std::vector<std::string>& domain);
                protected:
                    MongoDBLoggingDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBLoggingDataAccess();
                    
                public:
                    //! Inherited method
                    int insertNewEntry(data_access::LogEntry& log_entry);
                    
                    //! Inherited method
                    int searchEntryForSource(data_access::LogEntryList& entry_list,
                                             const std::string& source_uid,
                                             const std::vector<std::string>& domain,
                                             uint64_t start_sequence_id,
                                             uint32_t page_length);
                    //! Inherited method
                    int getLogDomainsForSource(data_access::LogDomainList& entry_list,
                                               const std::string& source_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__MongoDBLoggingDataAccess_h */
