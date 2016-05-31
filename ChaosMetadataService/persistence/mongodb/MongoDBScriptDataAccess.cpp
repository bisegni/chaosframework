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
using namespace chaos::service_common::data::dataset;
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
        
        builder << CHAOS_SBD_NAME << script_entry.script_description.name
        << CHAOS_SBD_DESCRIPTION << script_entry.script_description.description;
        
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
            err = updateScript(script_entry);
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::updateScript(Script& script) {
    int err = 0;
    mongo::BSONObjBuilder query_builder;
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q = BSON("seq"<< (long long)script.script_description.unique_id);
        
        query_builder << "seq"<< (long long)script.script_description.unique_id;
        if(script.script_description.name.size()){query_builder << CHAOS_SBD_NAME << script.script_description.name;}
        if(script.script_description.name.size()){query_builder << CHAOS_SBD_DESCRIPTION << script.script_description.description;}
        if(script.script_description.name.size()){query_builder << CHAOS_SBD_LANGUAGE << script.script_description.language;}
        
        //load script content
        if(script.script_content.size()){query_builder << CHAOS_SBD_SCRIPT_CONTENT << script.script_content;}
        
        if(script.dataset_attribute_list.size()) {
            //decalre serialization wrapper
            CHAOS_DECLARE_SD_WRAPPER_VAR(chaos::service_common::data::dataset::DatasetAttribute, da_dw);
            
            mongo::BSONArrayBuilder dataset_array_builder;
            
            
            int size = 0;
            for(DatasetAttributeListIterator it = script.dataset_attribute_list.begin(),
                end = script.dataset_attribute_list.end();
                it != end;
                it++) {
                
                //array builder for bson array
                da_dw = *it;
                std::auto_ptr<CDataWrapper> serialization = da_dw.serialize();
                mongo::BSONObj dataset_attribute(serialization->getBSONRawData(size));
                
                dataset_array_builder << dataset_attribute;
            }
            //add dataset array to update bson
            query_builder.appendArray(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION,
                                      dataset_array_builder.arr());
        }
        mongo::BSONObj u = query_builder.obj();
        
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

int MongoDBScriptDataAccess::searchScript(ScriptBaseDescriptionListWrapper& script_list,
                                          const std::string& search_string,
                                          uint64_t last_sequence_id,
                                          uint32_t page_length) {
    int err = 0;
    SearchResult paged_result;
    mongo::BSONObj p = BSON(CHAOS_SBD_NAME<< 1<<CHAOS_SBD_DESCRIPTION<<1);
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::Query q = getNextPagedQuery(last_sequence_id,
                                           search_string);
        
        DEBUG_CODE(SDA_DBG<<log_message("searchScript",
                                        "search",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = performPagedQuery(paged_result,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                    q,
                                    &p,
                                    NULL,
                                    page_length))) {
            SDA_ERR << "Error calling performPagedQuery with error" << err;
        } else {
            DEBUG_CODE(SDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
            if(paged_result.size()) {
                for (SearchResultIterator it = paged_result.begin();
                     it != paged_result.end();
                     it++) {
                    CDataWrapper element_found(it->objdata());
                    script_list.add(&element_found);
                    
                }
            }
        }
        
        //now all other part of the script are managed with update
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::loadScript(const chaos::service_common::data::script::ScriptBaseDescription& script_base_description,
                                        chaos::service_common::data::script::Script& script,
                                        bool load_source_code) {
    int err = 0;
    mongo::BSONObj element_found;
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q = BSON("seq" << (long long)script_base_description.unique_id
                                << CHAOS_SBD_NAME << script_base_description.name);
        
        DEBUG_CODE(SDA_DBG<<log_message("loadScript",
                                        "query",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = connection->findOne(element_found,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                      q))) {
            SDA_ERR << CHAOS_FORMAT("Error executin query for load script %1%[%2%] with error [%3%]", %script_base_description.unique_id%script_base_description.name%err);
        } else {
            if(element_found.isEmpty()) {
                SDA_ERR << CHAOS_FORMAT("The script %1%[%2%] has not been found", %script_base_description.unique_id%script_base_description.name);
            } else {
                // fill script with base description
                script.script_description.unique_id = (uint64_t)element_found.getField("seq").numberLong();
                if(element_found.hasField(CHAOS_SBD_NAME)){script.script_description.name = element_found.getField(CHAOS_SBD_NAME).String();};
                if(element_found.hasField(CHAOS_SBD_DESCRIPTION)){script.script_description.description = element_found.getField(CHAOS_SBD_DESCRIPTION).String();}
                if(element_found.hasField(CHAOS_SBD_LANGUAGE)){script.script_description.language = element_found.getField(CHAOS_SBD_LANGUAGE).String();}
                
                //load script content
                if(load_source_code && element_found.hasField(CHAOS_SBD_SCRIPT_CONTENT)){script.script_content = element_found.getField(CHAOS_SBD_SCRIPT_CONTENT).String();}
            }
        }
        
        //now all other part of the script are managed with update
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

#pragma Private Methods
mongo::Query MongoDBScriptDataAccess::getNextPagedQuery(uint64_t last_sequence,
                                                        const std::string& search_string) {
    mongo::Query q;
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONObjBuilder build_query_or;
    mongo::BSONArrayBuilder bson_find_and;
    
    if(last_sequence){bson_find_and <<BSON("seq" << BSON("$lt"<<(long long)last_sequence));}
    bson_find_and << BSON("$or" << getSearchTokenOnFiled(search_string, CHAOS_SBD_NAME));
    bson_find.appendArray("$and", bson_find_and.obj());
    q = bson_find.obj();
    return q.sort(BSON("seq"<<(int)1));
}

