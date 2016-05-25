/*
 *	MongoDBScriptDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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

#include "MongoDBScriptDataAccess.h"
#include "mongo_db_constants.h"

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define SDA_INFO    INFO_LOG(MongoDBScriptDataAccess)
#define SDA_DBG     DBG_LOG(MongoDBScriptDataAccess)
#define SDA_ERR     ERR_LOG(MongoDBScriptDataAccess)

MongoDBScriptDataAccess::MongoDBScriptDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
ScriptDataAccess(){}

MongoDBScriptDataAccess::~MongoDBScriptDataAccess() {}

int MongoDBScriptDataAccess::insertNewScript(Script& script_entry) {
    int err = 0;
    uint64_t sequence_id;
    mongo::BSONObjBuilder builder;
    CHAOS_ASSERT(utility_data_access)
    try {
        //start adding new base entry for the script
        //composed only with name and desription
        if(script_entry.script_description.name.size() == 0) {
            SDA_ERR << "Script name is not valid";
            return -1;
        }
        if(script_entry.script_description.description.size() == 0) {
            SDA_ERR << "Script description is not valid";
            return -2;
        }
        
        CHAOS_ASSERT(utility_data_access)
        if(utility_data_access->getNextSequenceValue("script", sequence_id)) {
            SDA_ERR << "Error getting new sequence for node";
            return err;
        } else {
            builder << "seq" << (long long)sequence_id;
        }
        
        builder << SBD_NAME << script_entry.script_description.name
        << SBD_DESCRIPTION << script_entry.script_description.name;
        
        mongo::BSONObj i = builder.obj();
        DEBUG_CODE(SDA_DBG<<log_message("insertNewScript",
                                        "insert",
                                        DATA_ACCESS_LOG_1_ENTRY("Insert",
                                                                i));)
        //inset on database new script description
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                     i))) {
            SDA_ERR << "Error creating new script";
        }
        
        //now all other part of the script are managed with update
        if(script_entry.script_content.size()) {
            //add sccript content
            err = updateScriptContent(script_entry.script_description,
                                      script_entry.script_content);
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::updateScriptContent(ScriptBaseDescription& script_identification,
                                                 const std::string script_content) {
    int err = 0;
    mongo::BSONObjBuilder query_builder;
    CHAOS_ASSERT(utility_data_access)
    try {
        
        query_builder << SBD_NAME << script_identification.name
        << SBD_DESCRIPTION << script_identification.name;
        
        mongo::BSONObj q = query_builder.obj();
        mongo::BSONObj u = BSON(SBD_SCRIPT_CONTENT<<script_content);
        
        DEBUG_CODE(SDA_DBG<<log_message("updateScriptContent",
                                        "update",
                                        DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                "Update",
                                                                q,
                                                                u));)
        //inset on database new script description
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                     q,
                                     u))) {
            SDA_ERR << "Error Updating sript content";
        }
        
        //now all other part of the script are managed with update
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! Inherited Method
int MongoDBScriptDataAccess::searchScript(chaos::service_common::data::script::ScriptList& script_list,
                                          const std::string& search_string,
                                          uint64_t start_sequence_id,
                                          uint32_t page_length) {
    return 0;
}

mongo::Query MongoDBScriptDataAccess::getNextPagedQuery(uint64_t last_sequence,
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
