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

#include <chaos/common/bson/util/base64.h>
#include <chaos/common/utility/TimingUtil.h>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <json/json.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::data::dataset;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define SDA_INFO    INFO_LOG(MongoDBScriptDataAccess)
#define SDA_DBG     DBG_LOG(MongoDBScriptDataAccess)
#define SDA_ERR     ERR_LOG(MongoDBScriptDataAccess)

static const std::string    ESCAPE_REPLACE_STRING("\\\\&");
static const boost::regex   ESCAPE_REPLACE_SEARCH_REGEXP("([\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\])");
static const boost::regex   REGEX_EXECUTION_POOL_VALIDATION("[A-Za-z0-9_]+:[A-Za-z0-9\\/_]+");

MongoDBScriptDataAccess::MongoDBScriptDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
ScriptDataAccess(),
utility_data_access(NULL){}

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
        } else {
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
    int size = 0;
    mongo::BSONObjBuilder query_builder;
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q = BSON("seq"<< (long long)script.script_description.unique_id <<
                                CHAOS_SBD_NAME << script.script_description.name);
        
        //compose bson update
        ScriptSDWrapper s_dw;
        s_dw.dataWrapped() = script;
        
        ChaosUniquePtr<chaos::common::data::CDataWrapper> serialization = s_dw.serialize();
        mongo::BSONObj u(serialization->getBSONRawData(size));
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
    mongo::BSONObj p = BSON("seq"<<1<<CHAOS_SBD_NAME<< 1<<CHAOS_SBD_DESCRIPTION<<1);
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::Query q = getNextPagedQuery(last_sequence_id,
                                           search_string);
        
        DEBUG_CODE(SDA_DBG<<log_message("searchScript",
                                        "performPagedQuery",
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

//! Inherited Method
int MongoDBScriptDataAccess::addScriptInstance(const uint64_t seq,
                                               const std::string& script_name,
                                               const std::string& instance_name) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
        CDataWrapper node_description;
        node_description.addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, instance_name);
        node_description.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
        node_description.addStringValue(chaos::NodeDefinitionKey::NODE_SUB_TYPE, chaos::NodeType::NODE_SUBTYPE_SCRIPTABLE_EXECUTION_UNIT);
        node_description.addStringValue(chaos::NodeDefinitionKey::NODE_GROUP_SET, script_name);
        node_description.addInt64Value("script_seq", seq);
        //now all other part of the script are managed with update
        if((err = node_data_access->insertNewNode(node_description))){
            return err;
        }
        
        mongo::Query q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID<< instance_name<<
                              chaos::NodeDefinitionKey::NODE_GROUP_SET << script_name<<
                              "script_seq" << (long long)seq);
        //add base instance information control control unit implementation
        mongo::BSONObj u = BSON("$set" << BSON("script_bind_type" << ScriptBindTypeDisable <<
                                               "instance_description" << BSON("control_unit_implementation" << "ScriptableExecutionUnit")));
        
        DEBUG_CODE(SDA_DBG<<log_message("addScriptInstance",
                                        "update",
                                        DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                "Update",
                                                                q,
                                                                u));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, u))){
            SDA_ERR << "Error updating script instance node with default instance data" << err;
            return err;
        }
        
        //we have the script now get the dataset and attach it to the instance
        
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::updateBindType(const chaos::service_common::data::script::ScriptBaseDescription& script_base_descrition,
                                            const chaos::service_common::data::script::ScriptInstance& instance) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
        mongo::Query q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID<< instance.instance_name<<
                              chaos::NodeDefinitionKey::NODE_GROUP_SET << script_base_descrition.name<<
                              "script_seq" << (long long)script_base_descrition.unique_id);
        mongo::BSONObj u;
        if(instance.bind_type == ScriptBindTypeUnitServer) {
            if(instance.bind_node.size() == 0) {SDA_ERR << "Invalid bind node name";return -1;}
            u = BSON("$set" << BSON("script_bind_type" << (int32_t)instance.bind_type <<
                                    CHAOS_FORMAT("instance_description.%1%",%chaos::NodeDefinitionKey::NODE_PARENT) << instance.bind_node));
        } else {
            u = BSON("$set" << BSON("script_bind_type" << (int32_t)instance.bind_type));
        }
        DEBUG_CODE(SDA_DBG<<log_message("addScriptInstance",
                                        "update",
                                        DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                "Update",
                                                                q,
                                                                u));)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, u))){
            SDA_ERR << "Error updating script instance node with default instance data" << err;
            return err;
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! Inherited Method
int MongoDBScriptDataAccess::removeScriptInstance(const uint64_t seq,
                                                  const std::string& script_name,
                                                  const std::string& instance_name) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
        //now all other part of the script are managed with update
        mongo::Query q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID<< instance_name<<
                              chaos::NodeDefinitionKey::NODE_GROUP_SET << script_name<<
                              "script_seq" << (long long)seq);
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q))) {
            SDA_ERR << CHAOS_FORMAT("Error removing instance %1% for script %2% with code %3%", %instance_name%script_name%err);
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::searchScriptInstance(std::vector<ScriptInstance>& instance_list,
                                                  const std::string& script_name,
                                                  const std::string& search_string,
                                                  uint64_t start_sequence_id,
                                                  uint32_t page_length) {
    int err = 0;
    SearchResult paged_result;
    try {
        mongo::Query q = getNextPagedQueryForInstance(start_sequence_id,
                                                      script_name,
                                                      search_string);
        DEBUG_CODE(SDA_DBG<<log_message("searchScriptInstance",
                                        "performPagedQuery",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = performPagedQuery(paged_result,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                    q,
                                    NULL,
                                    NULL,
                                    page_length))) {
            SDA_ERR << "Error calling performPagedQuery with error" << err;
        } else {
            DEBUG_CODE(SDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
            if(paged_result.size()) {
                for (SearchResultIterator it = paged_result.begin();
                     it != paged_result.end();
                     it++) {
                    if(it->hasField("seq") &&
                       it->hasField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)){
                        ScriptInstance instance;
                        instance.instance_seq = (uint64_t)it->getField("seq").Long();
                        instance.instance_name = it->getField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID).String();
                        mongo::BSONElement bte = it->getFieldDotted("script_bind_type");
                        instance.bind_type = bte.ok()?static_cast<ScriptBindType>(bte.numberInt()):ScriptBindTypeDisable;
                        mongo::BSONElement bne = it->getFieldDotted(CHAOS_FORMAT("instance_description.%1%",%NodeDefinitionKey::NODE_PARENT));
                        instance.bind_node = bne.ok()?bne.String():"";
                        instance_list.push_back(instance);
                    }
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

int MongoDBScriptDataAccess::loadScript(const uint64_t unique_id,
                                        const std::string& name,
                                        chaos::service_common::data::script::Script& script,
                                        bool load_source_code) {
    int err = 0;
    mongo::BSONObj element_found;
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q = BSON("seq" << (long long)unique_id
                                << CHAOS_SBD_NAME << name);
        
        DEBUG_CODE(SDA_DBG<<log_message("loadScript",
                                        "findOne",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = connection->findOne(element_found,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                      q))) {
            SDA_ERR << CHAOS_FORMAT("Error executin query for load script %1%[%2%] with error [%3%]", %unique_id%name%err);
        } else {
            if(element_found.isEmpty()) {
                SDA_ERR << CHAOS_FORMAT("The script %1%[%2%] has not been found", %unique_id%name);
            } else {
                // fill script with base description
                ScriptSDWrapper s_dw;
                CDataWrapper element_found_cdw(element_found.objdata());
                s_dw.deserialize(&element_found_cdw);
                script = s_dw.dataWrapped();
            }
        }
        
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::deleteScript(const uint64_t unique_id,
                                          const std::string& name) {
    int err = 0;
    mongo::BSONObj element_found;
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q_script = BSON("seq" << (long long)unique_id
                                       << CHAOS_SBD_NAME << name);
        mongo::BSONObj q_instance = BSON("script_seq" << (long long)unique_id
                                         << chaos::NodeDefinitionKey::NODE_GROUP_SET << name);
        DEBUG_CODE(SDA_DBG<<log_message("deleteScript",
                                        "remove[instance]",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q_instance));)
        //inset on database new script description
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q_instance))) {
            SDA_ERR << CHAOS_FORMAT("Error removing instance for script %1%[%2%] with error [%3%]", %unique_id%name%err);
        } else {
            DEBUG_CODE(SDA_DBG<<log_message("deleteScript",
                                            "remove[script]",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q_script));)
            //inset on database new script description
            if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                         q_script))) {
                SDA_ERR << CHAOS_FORMAT("Error removing script %1%[%2%] with error [%3%]", %unique_id%name%err);
            }
        }
        
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::getScriptForExecutionPoolPathList(const ChaosStringVector& pool_path,
                                                               std::vector<ScriptBaseDescription>& script_found,
                                                               uint64_t last_sequence_id,
                                                               uint32_t max_result) {
    int err = 0;
    //given a list of execution pool path the list of the script is returned. Every script into the
    //list can belog to one or more of the path present in the input list
    //the key execution_pool_list need to have one or more of the value present in the list "field : { $in : array }"
    SearchResult paged_result;
    mongo::BSONArrayBuilder bson_find_in;
    try {
        BOOST_FOREACH( std::string pool_path_element, pool_path ) {
            if(boost::regex_match(pool_path_element,
                                  REGEX_EXECUTION_POOL_VALIDATION) == false) continue;
            
            //escape xecution ppool path for earchunder the path
            const std::string result = boost::regex_replace(pool_path_element,
                                                            ESCAPE_REPLACE_SEARCH_REGEXP,
                                                            ESCAPE_REPLACE_STRING,
                                                            boost::match_default | boost::format_sed);
            bson_find_in.appendRegex(CHAOS_FORMAT("%1%[A-Za-z0-9\\/_]*",%result));
        }
        mongo::Query q = BSON("execution_pool_list" << BSON("$in"<< bson_find_in.arr()) <<
                              "seq" << BSON("$gt" << (long long)last_sequence_id) <<  "script_bind_type" << (int32_t)ScriptBindTypeAuto);
        //ordered by sequence
        q = q.sort(BSON("seq"<< 1));
        
        DEBUG_CODE(SDA_DBG<<log_message("getScriptForExecutionPoolPathList",
                                        "find",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        connection->findN(paged_result,
                          MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                          q,
                          max_result);
        if(paged_result.size()) {
            ScriptBaseDescriptionSDWrapper scrpt_desc_dw;
            //iterate for each elemente deserializing it with the data wrapper
            BOOST_FOREACH( mongo::BSONObj element, paged_result ) {
                CDataWrapper dw(element.objdata());
                scrpt_desc_dw.deserialize(&dw);
                script_found.push_back(scrpt_desc_dw.dataWrapped());
            }
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    
    return err;
}


int MongoDBScriptDataAccess::getUnscheduledInstanceForJob(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                          ChaosStringVector& instance_found_list,
                                                          uint32_t timeout,
                                                          uint32_t max_result) {
    int err = 0;
    SearchResult paged_result;
    mongo::BSONArrayBuilder bson_find_and;
    mongo::BSONArrayBuilder bson_find_hb_or;
    CHAOS_ASSERT(utility_data_access)
    try {
        //calucate the millisecond that we want to consider timeout
        mongo::Date_t start_timeout_date(TimingUtil::getTimeStamp() - timeout);
        bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_GROUP_SET << script.name);
        bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_TYPE << chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
        bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_SUB_TYPE << chaos::NodeType::NODE_SUBTYPE_SCRIPTABLE_EXECUTION_UNIT);
        bson_find_and << BSON("script_seq" << (long long)script.unique_id);
        bson_find_and << BSON("instance_description.auto_load"  << true);
        bson_find_and << BSON("script_bind_type"  << ScriptBindTypeAuto);
        bson_find_hb_or << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << BSON("$exists" << true << "$lt" << start_timeout_date));
        bson_find_hb_or << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << BSON("$exists" << false));
        bson_find_and << BSON("$or" << bson_find_hb_or.arr());
        
        mongo::BSONObj q = BSON("$and" << bson_find_and.arr());
        
        DEBUG_CODE(SDA_DBG<<log_message("instanceHeartbeat",
                                        "findN",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        connection->findN(paged_result,
                          MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                          q,
                          max_result);
        
        //!return instance
        BOOST_FOREACH( mongo::BSONObj element, paged_result ) {
            if(element.hasField(NodeDefinitionKey::NODE_UNIQUE_ID) == false) continue;
            instance_found_list.push_back(element.getStringField(NodeDefinitionKey::NODE_UNIQUE_ID));
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}


int MongoDBScriptDataAccess::reserveInstanceForScheduling(bool& reserverd,
                                                          const std::string& instance_uid,
                                                          const std::string& unit_server_parent,
                                                          uint32_t timeout) {
    int err = 0;
    //we need to use find adn update to perform prenotation of the instance uid, the rule are
    // instance is in timeout of not have the heartbeat
    mongo::BSONObj result;
    mongo::BSONArrayBuilder bson_find_and;
    mongo::BSONArrayBuilder bson_find_hb_or;
    CHAOS_ASSERT(utility_data_access)
    try {
        reserverd = false;
        
        //unique id rule
        bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << instance_uid);
        bson_find_and << BSON("script_bind_type"  << ScriptBindTypeAuto);
        //timeout rule
        uint64_t now = TimingUtil::getTimeStamp();
        mongo::Date_t start_timeout_date(now + timeout); //date need to be in the future because we need to reserve it
        bson_find_hb_or << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << BSON("$exists" << true << "$lt" << start_timeout_date));
        bson_find_hb_or << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << BSON("$exists" << false));
        bson_find_and << BSON("$or" << bson_find_hb_or.arr());
        
        mongo::BSONObj q = BSON("$and" << bson_find_and.arr());
        //the reservation is done using updateing the heartbeat and unit server alias
        mongo::BSONObj u = BSON("$set" << BSON(CHAOS_FORMAT("instance_description.%1%",%chaos::NodeDefinitionKey::NODE_PARENT) << unit_server_parent <<
                                               chaos::NodeDefinitionKey::NODE_TIMESTAMP << mongo::Date_t(now)));
        
        DEBUG_CODE(SDA_DBG<<log_message("instanceHeartbeat",
                                        "update",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = connection->findAndModify(result,
                                            MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                            q,
                                            u))){
            SDA_ERR << CHAOS_FORMAT("Error executin reservation on script instance %1% with error %2%", %instance_uid%err);
        } else {
            reserverd = (result.isEmpty() == false);
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::copyScriptDatasetAndContentToInstance(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                                   const std::string& script_instance) {
    int err = 0;
    mongo::BSONObj script_found;
    mongo::BSONObj instance_found;
    
    CHAOS_ASSERT(utility_data_access)
    try {
        mongo::BSONObj q = BSON("seq" << (long long)script.unique_id
                                << CHAOS_SBD_NAME << script.name);
        
        DEBUG_CODE(SDA_DBG<<log_message("copyScriptDatasetToInstance",
                                        "findOne[script]",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q));)
        //inset on database new script description
        if((err = connection->findOne(script_found,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SCRIPT),
                                      q))) {
            SDA_ERR << CHAOS_FORMAT("Error executin query for load script %1%[%2%] with error [%3%]", %script.unique_id%script.name%err);
        } else {
            if(script_found.isEmpty()) {
                SDA_ERR << CHAOS_FORMAT("The script %1%[%2%] has not been found", %script.unique_id%script.name);
            } else {
                //check the dataset presence within th script
                if(script_found.hasElement(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION) == false) {
                    err = -100;
                    SDA_ERR << "The script ans't any dataset configured";
                } else {
                    //now we need to search the instance and verify that it si conencted to the script so we need one query bson objet
                    q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << script_instance <<
                             chaos::NodeDefinitionKey::NODE_TYPE << chaos::NodeType::NODE_TYPE_CONTROL_UNIT <<
                             chaos::NodeDefinitionKey::NODE_SUB_TYPE << chaos::NodeType::NODE_SUBTYPE_SCRIPTABLE_EXECUTION_UNIT <<
                             chaos::NodeDefinitionKey::NODE_GROUP_SET << script.name <<
                             "script_seq" << (long long)script.unique_id);
                    
                    DEBUG_CODE(SDA_DBG<<log_message("copyScriptDatasetToInstance",
                                                    "findOne[instance for script]",
                                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                            q));)
                    
                    if((err = connection->findOne(instance_found,
                                                  MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                                  q))) {
                        SDA_ERR << CHAOS_FORMAT("Error executin query for load instance %1% for script %2%[%3%] with error [%4%]", %script_instance%script.unique_id%script.name%err);
                    } else {
                        if(instance_found.isEmpty()) {
                            SDA_ERR << CHAOS_FORMAT("The instance %1% for script %2%[%3%] has not been found", %script.name%script_instance%script.unique_id);
                        } else {
                            //now we can copy the dataset to the instance from his script
                            mongo::BSONObjBuilder ub;
                            //copy dataset
                            //                            ub << chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION << BSON(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP << mongo::Date_t(TimingUtil::getTimeStamp()) <<
                            //                                                                                                                chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION << script_found.getField(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
                            
                            //ccompose the laod parameter for describe the script language and content
                            const std::string script_language = script_found.getField(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE).String();
                            const std::string script_content = script_found.getField(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT).String();
                            
                            //load parameter need to have script and dataset
                            Json::FastWriter fast_writer;
                            Json::Value script_load_parameter;
                            script_load_parameter[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE] = script_language;
                            script_load_parameter[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT] = script_content;
                            
                            
                            CDataWrapper dataset_description;
                            //add node uid
                            dataset_description.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, script_instance);
                            
                            std::vector<mongo::BSONElement> dataset_element = script_found.getField(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION).Array();
                            
                            CDataWrapper node_dataset;
                            for(std::vector<mongo::BSONElement>::iterator it = dataset_element.begin(),
                                end = dataset_element.end();
                                it != end;
                                it++) {
                                CDataWrapper dataset_element(it->embeddedObject().objdata());
                                node_dataset.appendCDataWrapperToArray(dataset_element);
                            }
                            node_dataset.finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
                            dataset_description.addCSDataValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, node_dataset);
                            
                            
                            int bin_len;
                            const char * bin_data = dataset_description.getBSONRawData(bin_len);
                            const std::string encoded_binary_ds = bson::base64::encode(bin_data, bin_len);
                            
                            script_load_parameter[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION] = encoded_binary_ds;
                            ub.append(CHAOS_FORMAT("instance_description.%1%",%ControlUnitNodeDefinitionKey::CONTROL_UNIT_LOAD_PARAM), fast_writer.write(script_load_parameter));
                            
                            //copy dataset to update bson objet
                            mongo::BSONObj u = BSON("$set" << ub.obj());
                            
                            DEBUG_CODE(SDA_DBG<<log_message("copyScriptDatasetToInstance",
                                                            "update",
                                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                                    "Update",
                                                                                    q,
                                                                                    u));)
                            if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                                         q,
                                                         u))){
                                SDA_ERR << CHAOS_FORMAT("Error updating instance %1% for script %2%[%3%] with error [%4%]", %script_instance%script.unique_id%script.name%err);
                            }
                        }
                    }
                }
            }
        }
    } catch (const mongo::DBException &e) {
        SDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBScriptDataAccess::instanceForUnitServerHeartbeat(const ChaosStringVector& script_instance_list,
                                                            const std::string& unit_server_parent,
                                                            uint32_t timeout) {
    int err = 0;
    mongo::BSONArrayBuilder bson_find_and;
    mongo::BSONArrayBuilder bson_find_in;
    CHAOS_ASSERT(utility_data_access)
    try {
        for(ChaosStringVectorConstIterator in_it = script_instance_list.begin(),
            in_end = script_instance_list.end();
            in_it < in_end;
            in_it++) {
            //add every instance uid to the array
            bson_find_in << *in_it;
        }
        uint64_t now = TimingUtil::getTimeStamp();
        mongo::Date_t start_timeout_date(now-timeout);
        
        //and rule for uid
        bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << BSON("$in" << bson_find_in.arr()));
        
        //! for this unit server associaito
        bson_find_and << BSON(CHAOS_FORMAT("instance_description.%1%",%chaos::NodeDefinitionKey::NODE_PARENT) << unit_server_parent);
        
        //not in timeout
        //bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << BSON("$exists" << true << "$lte" << start_timeout_date));
        
        //compose the query
        mongo::BSONObj q = BSON("$and" << bson_find_and.arr());
        //we need to update only the timeout
        mongo::BSONObj u = BSON("$set" << BSON(chaos::NodeDefinitionKey::NODE_TIMESTAMP << mongo::Date_t(now)));
        DEBUG_CODE(SDA_DBG<<log_message("instanceHeartbeat",
                                        "update",
                                        DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                "Update",
                                                                q,
                                                                u));)
        //inset on database new script description
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u,
                                     false,
                                     true))) {
            SDA_ERR << CHAOS_FORMAT("Error executin query for update running script intance with error [%1%]", %err);
        }
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

mongo::Query MongoDBScriptDataAccess::getNextPagedQueryForInstance(uint64_t last_sequence_before_this_page,
                                                                   const std::string& script_name,
                                                                   const std::string& search_string) {
    mongo::Query q;
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_find_or;
    mongo::BSONArrayBuilder bson_find_and;
    std::vector<std::string> criteria_token;
    
    if(last_sequence_before_this_page){bson_find_and <<BSON("seq" << BSON("$lt"<<(long long)last_sequence_before_this_page));}
    
    
    boost::split(criteria_token, search_string,
                 boost::is_any_of(" "),
                 boost::token_compress_on);
    for (std::vector<std::string>::iterator it = criteria_token.begin();
         it != criteria_token.end();
         it++) {
        bson_find_or <<  MONGODB_REGEX_ON_FILED(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ".*"+*it+".*");
    }
    
    bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_GROUP_SET << script_name);
    bson_find_and << BSON("$or" << bson_find_or.arr());
    bson_find.appendArray("$and", bson_find_and.obj());
    q = bson_find.obj();
    return q.sort(BSON("seq"<<(int)1));
}

