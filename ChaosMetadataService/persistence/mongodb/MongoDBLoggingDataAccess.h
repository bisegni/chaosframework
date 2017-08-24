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
                    
                    MongoDBUtilityDataAccess *utility_data_access;
                    //return the query for a page
                    mongo::Query getNextPagedQuery(uint64_t last_sequence_before_this_page,
                                                   const std::string& source_uid,
                                                   const std::vector<std::string>& domain);
                    
                    ChaosSharedPtr<data_access::LogEntry> getEntryByBSON(const mongo::BSONObj& entry_bson);
                protected:
                    MongoDBLoggingDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
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
                    int searchEntryAdvanced(data_access::LogEntryList& entry_list,
                                            const std::string& search_string,
                                            const std::vector<std::string>& domain,
                                            uint64_t start_timestamp,
                                            uint64_t end_timestamp,
                                            uint64_t start_sequence_id,
                                            uint32_t page_length);
                    //! Inherited method
                    int getLogDomainsForSource(data_access::LogDomainList& entry_list,
                                               const data_access::LogSourceList& source_uids);
                    
                    //!inherited method
                    int eraseLogBeforTS(const std::string& source_uid,
                                        uint64_t unit_ts);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__MongoDBLoggingDataAccess_h */
