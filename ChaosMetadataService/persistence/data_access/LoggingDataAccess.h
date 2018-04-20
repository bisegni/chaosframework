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

#ifndef __CHAOSFramework__LoggingDataAccess_h
#define __CHAOSFramework__LoggingDataAccess_h

#include "../persistence.h"

#include <chaos_service_common/data/script/Script.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, std::string, LoggingKeyValueStringMap)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, int64_t,     LoggingKeyValueInt64Map)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, int32_t,     LoggingKeyValueInt32Map)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, double,      LoggingKeyValueDoubleMap)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, bool,        LoggingKeyValueBoolMap)
                
                //! define and entry of the log
                struct LogEntry {
                    uint64_t                    sequence;
                    uint64_t                    ts;
                    std::string                 domain;
                    std::string                 subject;
                    std::string                 source_identifier;
                    LoggingKeyValueStringMap    map_string_value;
                    LoggingKeyValueInt64Map     map_int64_value;
                    LoggingKeyValueInt32Map     map_int32_value;
                    LoggingKeyValueDoubleMap    map_double_value;
                    LoggingKeyValueBoolMap      map_bool_value;
                };
                
                typedef  ChaosUniquePtr<LogEntry> LogEntryUniuePtr;
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<LogEntry>, LogEntryList);
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, LogDomainList);
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, LogSourceList);
                
                class LoggingDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                    
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    LoggingDataAccess();
                    
                    //!default destructor
                    ~LoggingDataAccess();
                    
                    LogEntryUniuePtr getNewLogEntry(const std::string& log_emitter,
                                                    const std::string& log_subject,
                                                    const std::string& log_domain);
                    
                    int logException(const std::string& log_emitter,
                                     const std::string& log_subject,
                                     const std::string& log_domain,
                                     const chaos::CException& ex);
                    
                    int logError(const std::string& log_emitter,
                                 const std::string& log_subject,
                                 const std::string& log_domain,
                                 const int32_t& error_code,
                                 const std::string& error_message,
                                 const std::string& error_domain);
                    
                    //! Insert a new entry in the log
                    /*!
                     \param structure that describe the log entry
                     */
                    virtual int insertNewEntry(LogEntry& log_entry) = 0;
                    
                    //!Perform a search on log entries
                    /*!
                     perform a simple search on node filtering on type
                     \param entry_list the list of the current page of the entries found
                     \param source_uid is the source that has created the entries
                     \param start_sequence_id is identified the sequence after wich we need to search
                     \param page_length is the maximum number of the element to return
                     */
                    virtual int searchEntryForSource(LogEntryList& entry_list,
                                                     const std::string& source_uid,
                                                     const std::vector<std::string>& domain,
                                                     uint64_t start_sequence_id,
                                                     uint32_t page_length) = 0;
                    
                    //! perform and advanced search on log entries
                    /*!
                     perform a simple search on node filtering on type
                     \param entry_list tge list of the current page of the entries found
                     \param search_string is the tetxt serached in all key of all entries
                     \param domain filter for specific domain
                     \param start_timestamp if > 0 will be used the timestamp after wich we consider the entries
                     \param end_timestamp if > 0 will be used the timestamp before wich we consider the entries
                     \param start_sequence_id is identified the sequence after wich we need to search
                     \param page_length is the maximum number of the element to return
                     */
                    virtual int searchEntryAdvanced(LogEntryList& entry_list,
                                                    const std::string& search_string,
                                                    const std::vector<std::string>& domain,
                                                    uint64_t start_timestamp,
                                                    uint64_t end_timestamp,
                                                    uint64_t start_sequence_id,
                                                    uint32_t page_length) = 0;
                    
                    //!Return all log domain found for a determinated source
                    /*!
                     \param entry_list the list that will contains the found log domains
                     \param source_uid is the source uid for wich we need to found the log domains
                     */
                    virtual int getLogDomainsForSource(LogDomainList& entry_list,
                                                       const LogSourceList& source_uids) = 0;
                    
                    //! remove all log for belog to the source id
                    virtual int eraseLogBeforTS(const std::string& source_uid,
                                                uint64_t unit_ts) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__LoggingDataAccess_h */
