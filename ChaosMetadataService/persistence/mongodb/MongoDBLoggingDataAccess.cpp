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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#define MDBLDA_INFO INFO_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_DBG  DBG_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_ERR  ERR_LOG(MongoDBLoggingDataAccess)

#define WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(b, miter, map, c, ss)\
for(miter it = log_entry.map.begin();\
it != log_entry.map.end();\
it++) {\
b << it->first << c(it->second);\
ss.append(boost::lexical_cast<std::string>(c(it->second))); \
ss.append("|"); \
}

#define READ_STRING_LOG_ATTRIBUTE_MAP_ON_ELELEMT(e, m)

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

MongoDBLoggingDataAccess::MongoDBLoggingDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
LoggingDataAccess(){}

MongoDBLoggingDataAccess::~MongoDBLoggingDataAccess() {}

int MongoDBLoggingDataAccess::insertNewEntry(LogEntry& log_entry) {
    int err = 0;
    std::string search_field("|");
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
        builder << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT << log_entry.subject;
        
        //ppend subject to the search string
        search_field.append(log_entry.subject);search_field.append("|");
        
        //add custom attribute in log entry
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, LoggingKeyValueStringMapIterator, map_string_value, , search_field);
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, LoggingKeyValueInt64MapIterator, map_int64_value, (long long), search_field);
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, LoggingKeyValueInt32MapIterator, map_int32_value, (int), search_field);
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, LoggingKeyValueDoubleMapIterator, map_double_value, , search_field);
        WRITE_LOG_ATTRIBUTE_MAP_ON_BUILDER(builder, LoggingKeyValueBoolMapIterator, map_bool_value, , search_field);
        
        //add key with the all custom variable ass
        builder << "advanced_search" << search_field;
        
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


boost::shared_ptr<LogEntry> MongoDBLoggingDataAccess::getEntryByBSON(const mongo::BSONObj& entry_bson) {
    std::vector<mongo::BSONElement> all_element;
    boost::shared_ptr<LogEntry> log_entry(new LogEntry());
    
    entry_bson.elems(all_element);
    for(std::vector<mongo::BSONElement>::iterator it_ele = all_element.begin();
        it_ele != all_element.end();
        it_ele++) {
        const std::string field_name = it_ele->fieldName();
        if(field_name.compare("seq") == 0) {
            log_entry->sequence = (uint64_t)it_ele->Long();
        } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP) == 0) {
            log_entry->ts = (uint64_t)it_ele->Date().asInt64();
        } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER) == 0) {
            log_entry->source_identifier = it_ele->String();
        } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN) == 0) {
            log_entry->domain = it_ele->String();
        } else if(field_name.compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT) == 0) {
            log_entry->subject = it_ele->String();
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
    return log_entry;
}

int MongoDBLoggingDataAccess::searchEntryForSource(LogEntryList& entry_list,
                                                   const std::string& source_uid,
                                                   const std::vector<std::string>& domain,
                                                   uint64_t start_sequence_id,
                                                   uint32_t page_length) {
    int err = 0;
    SearchResult paged_result;
    
    mongo::Query q = getNextPagedQuery(start_sequence_id,
                                       source_uid,
                                       domain);
    //remove search field from result
    mongo::BSONObj p = BSON("advanced_search"<< 0);
    DEBUG_CODE(MDBLDA_DBG<<log_message("searchEntryForSource",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_2_ENTRY("Query",
                                                               "Projection",
                                                               q.toString(),
                                                               p.jsonString()));)
    
    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_LOGGING),
                                q,
                                &p,
                                NULL,
                                page_length))) {
        MDBLDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBLDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                //add entry in the result list
                entry_list.push_back(getEntryByBSON(*it));
            }
        }
    }
    return err;
}

int MongoDBLoggingDataAccess::searchEntryAdvanced(LogEntryList& entry_list,
                                                  const std::string& search_string,
                                                  const std::vector<std::string>& domain,
                                                  uint64_t start_timestamp,
                                                  uint64_t end_timestamp,
                                                  uint64_t start_sequence_id,
                                                  uint32_t page_length) {
    int err = 0;
    SearchResult paged_result;
    std::string token_for_mongo;
    std::vector<std::string> criteria_token;
    
    mongo::BSONArrayBuilder bson_find_and;
    mongo::BSONArrayBuilder bson_find_token_or;
    mongo::BSONObjBuilder bson_query_builder;
    
    if(search_string.size()) {
        boost::split(criteria_token,
                     search_string,
                     boost::is_any_of(" "),
                     boost::token_compress_on);
        if(criteria_token.size()) {
            for (std::vector<std::string>::iterator it = criteria_token.begin();
                 it != criteria_token.end();
                 it++) {
                token_for_mongo = ".*"+*it+".*";
                bson_find_token_or <<  MONGODB_REGEX_ON_FILED("advanced_search", token_for_mongo);
            }
            
            //add query on single token
            bson_find_and << BSON("$or" << bson_find_token_or.arr());
        }
    }
    
    //add query for timestamp if ar > 0
    if(start_timestamp) {
        bson_find_and << BSON(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP << BSON("$gte" << mongo::Date_t(start_timestamp)));
    }
    
    if(end_timestamp) {
        bson_find_and << BSON(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP << BSON("$lte" << mongo::Date_t(end_timestamp)));
    }
    
    mongo::Query q;
    mongo::BSONArray query_arr = bson_find_and.arr();
    
    if(query_arr.isEmpty() == false) {
        q = BSON("$and" << query_arr);
    }
    
    //remove search field from result
    mongo::BSONObj p = BSON("advanced_search"<< 0);
    DEBUG_CODE(MDBLDA_DBG<<log_message("searchEntryForSource",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_2_ENTRY("Query",
                                                               "Projection",
                                                               q.toString(),
                                                               p.jsonString()));)
    
    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_LOGGING),
                                q,
                                &p,
                                NULL,
                                page_length))) {
        MDBLDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBLDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                //add entry in the result list
                entry_list.push_back(getEntryByBSON(*it));
            }
        }
    }
    return err;
}

int MongoDBLoggingDataAccess::getLogDomainsForSource(LogDomainList& entry_list,
                                                     const LogSourceList& source_uids) {
    int err = 0;
    std::vector<std::string> distinct_values;
    mongo::BSONObj distinct_result;
    try {
        mongo::BSONObjBuilder build_query;
        if(source_uids.size()) {
            mongo::BSONArrayBuilder or_builder;
            //add all domain into array
            for(LogSourceListConstIterator it = source_uids.begin();
                it != source_uids.end();
                it++) {
                or_builder.append(*it);
            }
            
            build_query << MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER << BSON("$in" << or_builder.arr());
        }
        
        mongo::Query q = build_query.obj();
        
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