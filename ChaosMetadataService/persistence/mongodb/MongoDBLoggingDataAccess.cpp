/*
 *	MongoDBLoggingDataAccess.cpp
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

#include "mongo_db_constants.h"
#include "MongoDBLoggingDataAccess.h"

#include <mongo/client/dbclient.h>

#define MDBLDA_INFO INFO_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_DBG  DBG_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_ERR  ERR_LOG(MongoDBLoggingDataAccess)

#define WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(b, miter, map, c)\
for(miter it = log_entry.map.begin();\
it != log_entry.map.end();\
it++) {\
b << it->first << c(it->second);\
}

#define READ_STRING_LOG_ATTRIBUTE_MAP_ON_ELELEMT(e, m)

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBLoggingDataAccess::MongoDBLoggingDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
LoggingDataAccess(){}

MongoDBLoggingDataAccess::~MongoDBLoggingDataAccess() {}

int MongoDBLoggingDataAccess::insertNewEntry(data_access::LogEntry& log_entry) {
    int err = 0;
    mongo::BSONObjBuilder builder;
    CHAOS_ASSERT(utility_data_access)
    try {
        if(!log_entry.source_identifier.size()) return -1;
        
        if(utility_data_access->getNextSequenceValue("logging", log_entry.sequence)) {
            MDBLDA_ERR << "Error getting new sequence for log";
            return err;
        }
        
        //add default log entry attribute
        builder << "seq" << (long long)log_entry.sequence;
        builder << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER << log_entry.source_identifier;
        builder << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP << mongo::Date_t(log_entry.ts);
        builder << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN << log_entry.domain;
        
        //add custom attribute in log entry
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, data_access::LoggingKeyValueStringMapIterator, map_string_value, );
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, data_access::LoggingKeyValueInt64MapIterator, map_int64_value, (long long));
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, data_access::LoggingKeyValueInt32MapIterator, map_int32_value, (int));
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, data_access::LoggingKeyValueDoubleMapIterator, map_double_value, );
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, data_access::LoggingKeyValueBoolMapIterator, map_bool_value, );
        
        mongo::BSONObj q = builder.obj();
        
        DEBUG_CODE(MDBLDA_DBG<<log_message("insertNewEntry",
                                           "insert",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q));)
        
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_LOGGING),
                                     q))) {
            MDBLDA_ERR << "Error creating new entry in log";
        }
    } catch (const mongo::DBException &e) {
        MDBLDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

mongo::Query MongoDBLoggingDataAccess::getNextPagedQuery(uint64_t last_sequence,
                                                         const std::string& source_uid,
                                                         const std::vector<std::string>& domain) {
    mongo::Query q;
    mongo::BSONObjBuilder build_query;
    if(last_sequence){build_query <<"seq" << BSON("$lt"<<(long long)last_sequence);}
    build_query << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER << source_uid;
    if(domain.size()) {
        mongo::BSONArrayBuilder or_builder;
        //add all domain into array
        for(std::vector<std::string>::const_iterator it = domain.begin();
            it != domain.end();
            it++) {
            or_builder.append(*it);
        }
        
        build_query << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN << BSON("$in" << or_builder.arr());
    }
    q = build_query.obj();
    return q.sort(BSON(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP<<(int)-1));
}

int MongoDBLoggingDataAccess::searchEntryForSource(data_access::LogEntryList& entry_list,
                                                   const std::string& source_uid,
                                                   const std::vector<std::string>& domain,
                                                   uint64_t start_sequence_id,
                                                   uint32_t page_length) {
    int err = 0;
    SearchResult paged_result;
    uint64_t last_found_sequence;
    
    mongo::Query q = getNextPagedQuery(start_sequence_id,
                                       source_uid,
                                       domain);
    DEBUG_CODE(MDBLDA_DBG<<log_message("searchEntryForSource",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_1_ENTRY("Query",
                                                               q.toString()));)
    
    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_LOGGING),
                                q,
                                NULL,
                                NULL,
                                page_length))) {
        MDBLDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBLDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                
                std::vector<mongo::BSONElement> all_element;
                boost::shared_ptr<data_access::LogEntry> log_entry(new data_access::LogEntry());
                
                it->elems(all_element);
                for(std::vector<mongo::BSONElement>::iterator it_ele = all_element.begin();
                    it_ele != all_element.end();
                    it_ele++) {
                    const std::string field_name = it_ele->fieldName();
                    if(field_name.compare("seq") == 0) {
                        last_found_sequence = log_entry->sequence = (uint64_t)it_ele->Long();
                    } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP) == 0) {
                        log_entry->ts = (uint64_t)it_ele->Date().asInt64();
                    } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER) == 0) {
                        log_entry->source_identifier = it_ele->String();
                    } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN) == 0) {
                        log_entry->domain = it_ele->String();
                    } else {
                        //custom log key
                        switch(it_ele->type()) {
                            case NumberInt:
                                log_entry->map_int64_value.insert(make_pair(field_name, it_ele->numberInt()));
                                break;
                            case NumberLong:
                                log_entry->map_int64_value.insert(make_pair(field_name, it_ele->numberLong()));
                                break;
                            case NumberDouble:
                                log_entry->map_double_value.insert(make_pair(field_name, it_ele->numberDouble()));
                                break;
                            case String:
                                log_entry->map_string_value.insert(make_pair(field_name, it_ele->String()));
                                break;
                            case Bool:
                                log_entry->map_bool_value.insert(make_pair(field_name, it_ele->boolean()));
                                break;
                                
                            default:
                                break;
                                
                        }
                    }
                }
                
                //add entry in the result list
                entry_list.push_back(log_entry);
            }
        }
    }
    return err;
}

int MongoDBLoggingDataAccess::getLogDomainsForSource(data_access::LogDomainList& entry_list,
                                                     const std::string& source_uid) {
    int err = 0;
    std::vector<std::string> distinct_values;
    mongo::BSONObj distinct_result;
    try {
        mongo::Query q = BSON(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER << source_uid);
        DEBUG_CODE(MDBLDA_DBG<<log_message("getLogDomainsForSource",
                                           "query",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   "no query"));)
        
        distinct_result = connection->distinct(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_LOGGING), MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
        
        if(!distinct_result.isEmpty() &&
           distinct_result.couldBeArray()){
            distinct_result.Vals<std::string>(entry_list);
        }
    } catch (const mongo::DBException &e) {
        MDBLDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBLDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}