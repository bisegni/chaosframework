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
#include <mongo/client/dbclient.h>
#include "MongoDBNodeDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/utility/TimingUtil.h>
#include <boost/algorithm/string.hpp>

#include <map>

#define MDBNDA_INFO INFO_LOG(MongoDBNodeDataAccess)
#define MDBNDA_DBG  DBG_LOG(MongoDBNodeDataAccess)
#define MDBNDA_ERR  ERR_LOG(MongoDBNodeDataAccess)

using namespace boost;
using namespace chaos::common::property;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBNodeDataAccess::MongoDBNodeDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
utility_data_access(NULL){}

MongoDBNodeDataAccess::~MongoDBNodeDataAccess() {}

mongo::BSONObj MongoDBNodeDataAccess::getAliveOption(unsigned int timeout_sec) {
    return BSON(CHAOS_FORMAT("health_stat.%1%",%NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) << BSON("$gte" << mongo::Date_t(TimingUtil::getTimeStamp()-(timeout_sec*1000))));
}

int MongoDBNodeDataAccess::getNodeDescription(const std::string& node_unique_id,
                                              chaos::common::data::CDataWrapper **node_description) {
    int err = 0;
    mongo::BSONObj result;
    mongo::BSONObjBuilder query_builder;
    try {
        if(node_unique_id.size() == 0) return -1; //invalid unique id
        query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        
        mongo::BSONObj q = query_builder.obj();
        mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
                                chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
                                chaos::NodeDefinitionKey::NODE_SUB_TYPE << 1 <<
                                chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
                                chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
                                chaos::NodeDefinitionKey::NODE_HOST_NAME << 1 <<
                                chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
                                chaos::NodeDefinitionKey::NODE_TIMESTAMP << 1);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("getNodeDescription",
                                           "findOne",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Projection",
                                                                   q.jsonString(),
                                                                   p.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, &p))){
            MDBNDA_ERR << "Error fetching node description";
        } else if(result.isEmpty()) {
            MDBNDA_ERR << "No node description has been found";
            err = -2;
        } else {
            *node_description = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//inherited method
int MongoDBNodeDataAccess::insertNewNode(CDataWrapper& node_description) {
    int err = 0;
    uint64_t sequence_id = 0;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) return -2;
        if(!node_description.hasKey("seq")) {
            CHAOS_ASSERT(utility_data_access)
            if(utility_data_access->getNextSequenceValue("nodes", sequence_id)) {
                MDBNDA_ERR << "Error getting new sequence for node";
                return err;
            } else {
                node_description.addInt64Value("seq", sequence_id);
            }
        }
        
        ChaosUniquePtr<SerializationBuffer> ser(node_description.getBSONData());
        mongo::BSONObj obj_to_insert(ser->getBufferPtr());
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("insertNewNode",
                                           "insert",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   obj_to_insert));)
        
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     obj_to_insert))) {
            MDBNDA_ERR << "Error creating new node";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! update the node updatable feature
int MongoDBNodeDataAccess::updateNode(chaos::common::data::CDataWrapper& node_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    mongo::BSONArrayBuilder bson_update_array;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        
        //serach criteria
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_TYPE);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_SUB_TYPE)) {
            updated_field << chaos::NodeDefinitionKey::NODE_SUB_TYPE << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_SUB_TYPE);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
            updated_field << chaos::NodeDefinitionKey::NODE_RPC_ADDR << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) {
            updated_field << chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_HOST_NAME)) {
            updated_field << chaos::NodeDefinitionKey::NODE_HOST_NAME << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_HOST_NAME);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)) {
            updated_field << chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) {
            updated_field << chaos::NodeDefinitionKey::NODE_TIMESTAMP << mongo::Date_t(node_description.getUInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP));
        }
        if(node_description.hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC)) {
            ChaosUniquePtr<CMultiTypeDataArrayWrapper> description_array(node_description.getVectorValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC));
            for(int desc_idx = 0;
                desc_idx < description_array->size();
                desc_idx++) {
                ChaosUniquePtr<CDataWrapper> desc_obj(description_array->getCDataWrapperElementAtIndex(desc_idx));
                if(desc_obj->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC)) {
                    ChaosUniquePtr<CMultiTypeDataArrayWrapper> action_array(desc_obj->getVectorValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC));
                    for(int idx = 0;
                        idx < action_array->size();
                        idx++) {
                        mongo::BSONObjBuilder action_description;
                        mongo::BSONArrayBuilder param_descriptions;
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> element(action_array->getCDataWrapperElementAtIndex(idx));
                        if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN)) {
                            action_description << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN
                            << element->getStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
                        }
                        if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME)) {
                            action_description << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
                            << element->getStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
                        }
                        if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION)) {
                            action_description << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION
                            << element->getStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION);
                        }
                        //check if the action has parameter
                        if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM)) {
                            ChaosUniquePtr<CMultiTypeDataArrayWrapper> param_array(element->getVectorValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM));
                            for(int idx = 0;
                                idx < param_array->size();
                                idx++) {
                                mongo::BSONObjBuilder single_param_desc;
                                ChaosUniquePtr<chaos::common::data::CDataWrapper> param(param_array->getCDataWrapperElementAtIndex(idx));
                                
                                if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME)) {
                                    single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME
                                    << param->getStringValue( chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME);
                                }
                                if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO)) {
                                    single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO
                                    << param->getStringValue( chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO);
                                }
                                if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE)) {
                                    single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE
                                    << param->getInt32Value(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE);
                                }
                                //add element to array
                                param_descriptions << single_param_desc.obj();
                            }
                            //add all param description
                            action_description.appendArray(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM, param_descriptions.arr());
                        }
                        //appen action description to array
                        bson_update_array << action_description.obj();
                    }
                }
            }
            //add all action to the bson update object
            updated_field.appendArray(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC, bson_update_array.arr());
        }
        if(updated_field.asTempObj().isEmpty()) return 0;
        mongo::BSONObj query = bson_find.obj();
        //set the update
        bson_update << "$set" << updated_field.obj();
        mongo::BSONObj update = bson_update.obj();
        
        
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("updateUS",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   query.jsonString(),
                                                                   update.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBNDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::checkNodePresence(bool& presence,
                                             const std::string& node_unique_id,
                                             const std::string& node_unique_type) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObj result;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        if(node_unique_type.size()>0) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_unique_type;
        }
        mongo::BSONObj q = bson_find.obj();
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("checkNodePresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q))){
            MDBNDA_ERR << "Error sercing unit server unit server" << node_unique_id;
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::setNodeHealthStatus(const std::string& node_unique_id,
                                               const HealthStat& health_stat) {
    int err = 0;
    
    try {
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id);
        mongo::BSONObj u = BSON("$set" << BSON("health_stat" << BSON(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP << mongo::Date_t(health_stat.timestamp) <<
                                                                     NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME << (long long)health_stat.uptime <<
                                                                     NodeHealtDefinitionKey::NODE_HEALT_USER_TIME << health_stat.user_time <<
                                                                     NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME << health_stat.sys_time <<
                                                                     NodeHealtDefinitionKey::NODE_HEALT_STATUS << health_stat.health_status)));
        DEBUG_CODE(MDBNDA_DBG<<log_message("setNodeHealthStatus",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "update",
                                                                   q.jsonString(),
                                                                   u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))){
            MDBNDA_ERR << CHAOS_FORMAT("Error updating health stat information for node %1%",%node_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getNodeHealthStatus(const std::string& node_unique_id,
                                               HealthStat& health_stat) {
    int err = 0;
    
    try {
        mongo::BSONObj result;
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id <<
                                "health_stat" << BSON("$exists" << true));
        DEBUG_CODE(MDBNDA_DBG<<log_message("setNodeHealthStatus",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q))){
            MDBNDA_ERR << CHAOS_FORMAT("Error getting health stat information for node %1%",%node_unique_id);
        } else if(result.isEmpty() == false) {
            //we have found health stat
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::deleteNode(const std::string& node_unique_id,
                                      const std::string& node_type) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        if(node_type.size()) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_type;
        }
        mongo::BSONObj q = bson_find.obj();
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteNode",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q))){
            MDBNDA_ERR << "Error deleting unit server" << node_unique_id;
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::searchNode(chaos::common::data::CDataWrapper **result,
                                      const std::string& criteria,
                                      uint32_t search_type,
                                      bool alive_only,
                                      uint32_t last_unique_id,
                                      uint32_t page_length) {
    int err = 0;
    //decode the type of the node
    std::string             type_of_node;
    //contain token found in criteria
    std::vector<std::string> criteria_token;
    //helper for regular expression
    std::string             token_for_mongo;
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_find_or;
    mongo::BSONArrayBuilder bson_find_and;
    SearchResult            paged_result;
    
    //compose query
    
    //filter on sequence
    bson_find_and << BSON( "seq" << BSON("$gt"<<last_unique_id));
    
    //filter on type
    if(search_type>0) {
        switch(search_type) {
            case 1:
                type_of_node = chaos::NodeType::NODE_TYPE_UNIT_SERVER;
                break;
            case 2:
                type_of_node = chaos::NodeType::NODE_TYPE_CONTROL_UNIT;
                break;
            case 3:
                type_of_node = chaos::NodeType::NODE_TYPE_AGENT;
                break;
            default:
                break;
        }
        bson_find_and << BSON( chaos::NodeDefinitionKey::NODE_TYPE << type_of_node);
    }
    
    if(alive_only){bson_find_and << getAliveOption(6);}
    
    //compose the 'or' condition for all token of unique_id filed
    bson_find_and << BSON("$or" << getSearchTokenOnFiled(criteria, chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    bson_find.appendArray("$and", bson_find_and.obj());
    mongo::BSONObj q = bson_find.obj();
    mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
                            chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
                            chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
                            "seq" << 1 <<
                            "health_stat" <<1);
    DEBUG_CODE(MDBNDA_DBG<<log_message("searchNode",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_2_ENTRY("Query",
                                                               "Projection",
                                                               q.jsonString(),
                                                               p.jsonString()));)
    
    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                q,
                                &p,
                                NULL,
                                page_length))) {
        MDBNDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBNDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            *result = new CDataWrapper();
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                std::string node_uid_found;
                try {
                    CDataWrapper cd;
                    cd.addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, node_uid_found = it->getField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID).String());
                    cd.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, it->getField(chaos::NodeDefinitionKey::NODE_TYPE).String());
                    if(cd.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
                        cd.addStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR, it->getField(chaos::NodeDefinitionKey::NODE_RPC_ADDR).String());
                    }
                    cd.addInt64Value("seq", (int64_t)it->getField("seq").Long());
                    
                    if(it->hasField("health_stat")) {
                        mongo::BSONElement health_stat_element = it->getField("health_stat");
                        if(health_stat_element.type() == mongo::Object) {
                            mongo::BSONObj o = health_stat_element.embeddedObject();
                            //we can retrieve the data
                            CDataWrapper health;
                            health.addInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, o.getField(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP).date().asInt64());
                            health.addInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, (int64_t)o.getField(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME).Long());
                            health.addDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, o.getField(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME).Double());
                            health.addDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME, o.getField(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME).Double());
                            health.addStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS, o.getField(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS).String());
                            cd.addCSDataValue("health_stat", health);
                        }
                    }
                    (*result)->appendCDataWrapperToArray(cd);
                } catch(...) {
                    MDBNDA_ERR << "Exception during scan of found node:" << node_uid_found;
                }
            }
            (*result)->finalizeArrayForKey("node_search_result_page");
        }
    }
    return err;
}

int MongoDBNodeDataAccess::setProperty(const std::string& node_uid,
                                       const PropertyGroupVector& property_group_vector) {
    int err = 0;
    int size = 0;
    try {
        chaos::common::property::PropertyGroupVectorSDWrapper pg_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroupVector, const_cast<PropertyGroupVector&>(property_group_vector)));
        pg_sdw.serialization_key = "property";
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_uid);
        mongo::BSONObj u = BSON("$set" << mongo::BSONObj(pg_sdw.serialize()->getBSONRawData(size)));
        DEBUG_CODE(MDBNDA_DBG<<log_message("setProperty",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   q.jsonString(),
                                                                   u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))){
            MDBNDA_ERR << CHAOS_FORMAT("Error setting property for %1%",%node_uid);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

PropertyGroup *getPrgFromVec(const std::string& group_name,
                             chaos::common::property::PropertyGroupVector& property_group_vector) {
    PropertyGroup *res_ptr = NULL;
    for(PropertyGroupVectorIterator it = property_group_vector.begin(),
        end = property_group_vector.end();
        it != end;
        it++) {
        if(it->getGroupName().compare(group_name) != 0) continue;
        res_ptr = &(*it);
        break;
    }
    return res_ptr;
}

//update porperty without removing old, if old are present the value are pushed
int MongoDBNodeDataAccess::updatePropertyDefaultValue(const std::string& node_uid,
                                                      const chaos::common::property::PropertyGroupVector& property_group_vector) {
    int err = 0;
    int size = 0;
    
    //get store default and update or add new
    PropertyGroupVector property_stored;
    if((err = getProperty(data_access::PropertyTypeDefaultValues,
                          node_uid, property_stored)) != 0) return err;
    
    for(PropertyGroupVectorConstIterator it = property_group_vector.begin(),
        end = property_group_vector.end();
        it != end;
        it++) {
        const std::string group_name = it->getGroupName();
        
        PropertyGroup *pg = getPrgFromVec(group_name, property_stored);
        if(pg != NULL) {
            pg->copyPropertiesFromGroup(*it, true);
        } else {
            property_stored.push_back(*it);
        }
    }
    //no we can update all defaults
    try {
        chaos::common::property::PropertyGroupVectorSDWrapper pg_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroupVector, const_cast<PropertyGroupVector&>(property_stored)));
        pg_sdw.serialization_key = "property_defaults";
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_uid);
        mongo::BSONObj u = BSON("$set" << mongo::BSONObj(pg_sdw.serialize()->getBSONRawData(size)));
        DEBUG_CODE(MDBNDA_DBG<<log_message("updatePropertyDefaultValue",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   q.jsonString(),
                                                                   u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))){
            MDBNDA_ERR << CHAOS_FORMAT("Error updating property default values for %1%",%node_uid);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getProperty(const data_access::PropertyType property_type,
                                       const std::string& node_uid,
                                       chaos::common::property::PropertyGroupVector& property_group_vector) {
    int err = 0;
    try {
        std::string property_key;
        switch (property_type) {
            case data_access::PropertyTypeDescription:
                property_key = "property";
                break;
                
            case data_access::PropertyTypeDefaultValues:
                property_key = "property_defaults";
                break;
        }
        mongo::BSONObj found_property;
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_uid <<
                                property_key << BSON("$exists" << true));
        mongo::BSONObj p = BSON(property_key << 1);
        DEBUG_CODE(MDBNDA_DBG<<log_message("getProperty",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(found_property,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q,
                                      &p))){
            MDBNDA_ERR << CHAOS_FORMAT("Error retriving property for %1%",%node_uid);
        }
        if(!found_property.isEmpty()) {
            CDWUniquePtr prop_ser(new CDataWrapper(found_property.objdata()));
            chaos::common::property::PropertyGroupVectorSDWrapper pg_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroupVector, property_group_vector));
            pg_sdw.serialization_key = property_key;
            pg_sdw.deserialize(prop_ser.get());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getPropertyGroup(const data_access::PropertyType property_type,
                                            const std::string& node_uid,
                                            const std::string& property_group_name,
                                            chaos::common::property::PropertyGroup& property_group) {
    int err = 0;
    try {
        std::string property_key;
        switch (property_type) {
            case data_access::PropertyTypeDescription:
                property_key = "property";
                break;
                
            case data_access::PropertyTypeDefaultValues:
                property_key = "property_defaults";
                break;
        }
        mongo::BSONObj found_property;
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_uid <<
                                property_key << BSON("$exists" << true) <<
                                CHAOS_FORMAT("%1%.property_g_name", %property_key) << property_group_name);
        mongo::BSONObj p = BSON("_id" << 0 << CHAOS_FORMAT("%1%", %property_key) << 1);
        DEBUG_CODE(MDBNDA_DBG<<log_message("getProperty",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(found_property,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q,
                                      &p))){
            MDBNDA_ERR << CHAOS_FORMAT("Error retriving property for %1%",%node_uid);
        }
        if(!found_property.isEmpty()) {
            CDWUniquePtr prop_ser(new CDataWrapper(found_property.objdata()));
            chaos::common::property::PropertyGroupVectorSDWrapper pg_sdw;
            pg_sdw.serialization_key = property_key;
            pg_sdw.deserialize(prop_ser.get());
            if(pg_sdw().size()) {
                property_group = pg_sdw()[0];
            }
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::checkCommandPresence(const std::string& command_unique_id,
                                                bool& presence) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("checkCommandPresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error checking presence for command uid:%1%")%command_unique_id);
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::setCommand(chaos::common::data::CDataWrapper& command) {
    int err = 0;
    mongo::BSONObj result;
    mongo::BSONObjBuilder update_builder;
    try {
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            return -1;
            MDBNDA_ERR << "No command uid found";
        }
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
            return -2;
            MDBNDA_ERR << "No command alias found";
        }
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)) {
            return -3;
            MDBNDA_ERR << "No command description found";
        }
        ChaosUniquePtr<SerializationBuffer> ser(command.getBSONData());
        mongo::BSONObj i(ser->getBufferPtr());
        
        mongo::BSONObj query = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command.getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID));
        
        update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_ALIAS));
        update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION));
        if(i.hasElement(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)){
            //we have parameters
            update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
        }
        mongo::BSONObj update = BSON("$set" << update_builder.obj());
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("updateUS",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   query.jsonString(),
                                                                   update.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                     query,
                                     update,
                                     true))) {
            MDBNDA_ERR << "Error updating unit server:" << err;
        }
        
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getCommand(const std::string& command_unique_id,
                                      chaos::common::data::CDataWrapper **command) {
    int err = 0;
    mongo::BSONObj result;
    try {
        //create query
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("getCommand",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching the command uid:%2%")%command_unique_id);
        } else if(result.isEmpty()) {
            MDBNDA_ERR << boost::str(boost::format("No command for uid:%2%")%command_unique_id);
        } else {
            //return a new datawrapper with the found content
            *command = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! inherited method
int MongoDBNodeDataAccess::deleteCommand(const std::string& command_unique_id) {
    int err = 0;
    try {
        //create query
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteCommandTemplate",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                     q))){
            MDBNDA_ERR << boost::str(boost::format("Error removing the command uid:%2%")%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::checkCommandTemplatePresence(const std::string& template_name,
                                                        const std::string& command_unique_id,
                                                        bool& presence) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("checkCommandTemplatePresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::setCommandTemplate(chaos::common::data::CDataWrapper& command_template) {
    int err = 0;
    try {
        if(!command_template.hasKey("template_name") ||
           !command_template.hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            MDBNDA_ERR << boost::str(boost::format("The key 'template_name' and  '%2%' are mandatory!")%"template_name"%BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
            return -1;
        }
        
        std::string template_name = command_template.getStringValue("template_name");
        std::string command_unique_id = command_template.getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
        
        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        //create the update package (all key imnus the first two used before
        ChaosUniquePtr<chaos::common::data::CDataWrapper> to_update(new CDataWrapper());
        to_update->addStringValue("type", "template");
        std::vector<std::string> all_keys;
        command_template.getAllKey(all_keys);
        
        for(std::vector<std::string>::iterator it = all_keys.begin();
            it != all_keys.end();
            it++) {
            if((it->compare("template_name")==0) ||
               (it->compare(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)==0)) continue;
            
            //copy the key if present
            command_template.copyKeyTo(*it, *to_update);
        }
        
        ChaosUniquePtr<SerializationBuffer> chaos_bson(to_update->getBSONData());
        mongo::BSONObj u = BSON("$set" << mongo::BSONObj(chaos_bson->getBufferPtr()));
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("setCommandTemplate",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "update",
                                                                   q.jsonString(),
                                                                   u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                     q,
                                     u,
                                     true))){
            MDBNDA_ERR << boost::str(boost::format("Error setting the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::deleteCommandTemplate(const std::string& template_name,
                                                 const std::string& command_unique_id) {
    int err = 0;
    try {
        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteCommandTemplate",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                     q))){
            MDBNDA_ERR << boost::str(boost::format("Error removing the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getCommandTemplate(const std::string& template_name,
                                              const std::string& command_unique_id,
                                              chaos::common::data::CDataWrapper **command_template) {
    int err = 0;
    mongo::BSONObj result;
    try {
        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("getCommandTemplate",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        } else if(result.isEmpty()) {
            MDBNDA_ERR << boost::str(boost::format("No template found for template name: %1% and command uid:%2%")%template_name%command_unique_id);
        } else {
            //return a new datawrapper with the found content
            *command_template = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! inherited method
int MongoDBNodeDataAccess::searchCommandTemplate(chaos::common::data::CDataWrapper **result,
                                                 const std::vector<std::string>& cmd_uid_to_filter,
                                                 uint32_t last_unique_id,
                                                 uint32_t page_length) {
    int err = 0;
    
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_uid_array_list;
    mongo::BSONArrayBuilder bson_find_and;
    SearchResult            paged_result;
    
    //compose query
    
    //filter on sequence
    bson_find_and << BSON( "seq" << BSON("$gte"<<last_unique_id));
    
    //filter on type
    for(std::vector<std::string>::const_iterator it = cmd_uid_to_filter.begin();
        it != cmd_uid_to_filter.end();
        it++) {
        bson_uid_array_list << *it;
    }
    
    //compose the or
    bson_find_and << BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << BSON("$in" << bson_uid_array_list.arr()));
    bson_find.appendArray("$and", bson_find_and.obj());
    mongo::BSONObj q = bson_find.obj();
    mongo::BSONObj p = BSON("template_name" << 1 <<
                            BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << 1);
    DEBUG_CODE(MDBNDA_DBG<<log_message("searchCommandTemplate",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_2_ENTRY("Query",
                                                               "Projection",
                                                               q.jsonString(),
                                                               p.jsonString()));)
    
    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                q,
                                NULL,
                                NULL,
                                page_length))) {
        MDBNDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBNDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            *result = new CDataWrapper();
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                CDataWrapper cd(it->objdata());
                (*result)->appendCDataWrapperToArray(cd);
            }
            (*result)->finalizeArrayForKey("cmd_tmpl_search_result_page");
        }
    }
    return err;
}

int MongoDBNodeDataAccess::addAgeingManagementDataToNode(const std::string& control_unit_id) {
    int err = 0;
    try {
        uint64_t current_ts = TimingUtil::getTimeStamp();
        const std::string key_processing_ageing = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PROCESSING_AGEING);
        const std::string key_last_checing_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_AGEING_LAST_CHECK_DATA);
        const std::string key_last_performed_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PERFORMED_AGEING);
        
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_id
                                    << MONGODB_COLLECTION_NODES_AGEING_INFO << BSON("$exists" << false ));
        
        mongo::BSONObj update = BSON("$set" << BSON(key_last_checing_time << mongo::Date_t(current_ts) <<
                                                    key_last_performed_time << mongo::Date_t(current_ts) <<
                                                    key_processing_ageing << false));
        
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("completeNodeForAgeingManagement",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("query",
                                                                   "update",
                                                                   query.jsonString(),
                                                                   update.jsonString()));)
        //remove the field of the document
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBNDA_DBG << CHAOS_FORMAT("Error %1% completing control unit %2% with ageing management information", %err%control_unit_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBNDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBNodeDataAccess::isNodeAlive(const std::string& node_uid, bool& alive) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << node_uid <<
                                    CHAOS_FORMAT("health_stat.%1%",%NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) <<
                                    BSON("$gte" << mongo::Date_t(TimingUtil::getTimestampWithDelay((6*1000)))));
        
        DEBUG_CODE(MDBNDA_DBG<<log_message("isNodeAlive",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("query",
                                                                   query.jsonString()));)
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query))) {
            MDBNDA_DBG << CHAOS_FORMAT("Error %1% determinating the alive state for node %2%", %err%node_uid);
        }
        alive = (result.isEmpty() == false);
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBNDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}
//int MongoDBNodeDataAccess::reserveNodeForAgeingManagement(uint64_t& last_sequence_id,
//                                                          std::string& node_uid_reserved,
//                                                          uint32_t& node_ageing_time,
//                                                          uint64_t& last_ageing_perform_time,
//                                                          uint64_t timeout_for_checking,
//                                                          uint64_t delay_next_check) {
//    int err = 0;
//    SearchResult paged_result;
//    try {
//        mongo::BSONObj result_found;
//        mongo::BSONObjBuilder query_builder;
//        mongo::BSONArrayBuilder query_ageing_and;
//        mongo::BSONArrayBuilder query_ageing_or;
//
//        const std::string key_processing_ageing = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PROCESSING_AGEING);
//        const std::string key_last_checking_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_AGEING_LAST_CHECK_DATA);
//        const std::string key_last_performed_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PERFORMED_AGEING);
//        //get all node where ageing is > of 0
//        query_builder << CHAOS_FORMAT("instance_description.%1%",%DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING) << BSON("$gt" << 0);
//
//        //get all control unit
//        query_builder << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT;
//
//        //condition on sequence
//        query_builder << "seq" << BSON("$gt" << (long long)last_sequence_id);
//
//        //select control unit also if it is in checking managemnt but data checking time is old than one minute(it is gone in timeout)
//        query_ageing_or << BSON(key_processing_ageing << true << key_last_checking_time << BSON("$lte" << mongo::Date_t(TimingUtil::getTimeStamp()-timeout_for_checking)));
//
//        //or on previous condition and on checking management == false the last checking date need to be greater that noral chack timeout
//        query_ageing_or << BSON(key_processing_ageing << false << key_last_checking_time << BSON("$lte" << mongo::Date_t(TimingUtil::getTimeStamp()-delay_next_check)));
//
//        query_builder << "$or" << query_ageing_or.arr();
//
//
//        mongo::BSONObj  query = query_builder.obj();
//        // set atomicalli processing ageing to true
//        mongo::BSONObj  update =  BSON("$set" << BSON(key_processing_ageing << true <<
//                                                      key_last_checking_time << mongo::Date_t(TimingUtil::getTimeStamp())));
//        // order getting first cu being the last processed one
//        mongo::BSONObj  order_by = BSON(key_last_checking_time << 1);
//        DEBUG_CODE(MDBNDA_DBG<<log_message("reserveNodeForAgeingManagement",
//                                           "findAndUpdate",
//                                           DATA_ACCESS_LOG_3_ENTRY("query",
//                                                                   "update",
//                                                                   "order_by",
//                                                                   query.jsonString(),
//                                                                   update.jsonString(),
//                                                                   order_by.jsonString()));)
//        //find the control unit
//        if((err = connection->findAndModify(result_found,
//                                            MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
//                                            query,
//                                            update,
//                                            false,
//                                            false,
//                                            order_by))){
//            MDBNDA_DBG << CHAOS_FORMAT("Error %1% fetching the next cheable control unit for ageing", %err);
//        } else if(result_found.isEmpty() == false && (result_found.hasField(NodeDefinitionKey::NODE_UNIQUE_ID) &&
//                                                      result_found.hasField(MONGODB_COLLECTION_NODES_AGEING_INFO))) {
//            //we have control unit
//            last_sequence_id = (uint64_t)result_found.getField("seq").Long();
//            node_uid_reserved = result_found.getField(NodeDefinitionKey::NODE_UNIQUE_ID).String();
//            node_ageing_time = (uint32_t)result_found.getFieldDotted(CHAOS_FORMAT("instance_description.%1%",%DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING)).numberInt();
//            last_ageing_perform_time = (uint64_t)result_found.getFieldDotted(key_last_performed_time).Date().asInt64();
//        } else {
//            last_sequence_id = 0;
//            node_uid_reserved.clear();
//            last_ageing_perform_time = 0;
//            node_ageing_time = 0;
//        }
//
//    } catch (const mongo::DBException &e) {
//        MDBNDA_ERR << e.what();
//        err = -1;
//    } catch (const CException &e) {
//        MDBNDA_ERR << e.what();
//        err = e.errorCode;
//    }
//    return err;
//}
//
//int MongoDBNodeDataAccess::releaseNodeForAgeingManagement(std::string& node_uid,
//                                                          bool performed) {
//    int err = 0;
//    SearchResult paged_result;
//    try {
//        mongo::BSONObj result_found;
//        uint64_t current_ts = TimingUtil::getTimeStamp();
//        const std::string key_processing_ageing = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PROCESSING_AGEING);
//        const std::string key_last_checking_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_AGEING_LAST_CHECK_DATA);
//        const std::string key_last_performed_time = CHAOS_FORMAT("%1%.%2%",%MONGODB_COLLECTION_NODES_AGEING_INFO%MONGODB_COLLECTION_NODES_PERFORMED_AGEING);
//
//        mongo::BSONObj  query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << node_uid);
//        // set atomicalli processing ageing to true
//        mongo::BSONObj  update = BSON("$set" << (performed?BSON(key_processing_ageing << false <<
//                                                                key_last_checking_time << mongo::Date_t(current_ts) <<
//                                                                key_last_performed_time << mongo::Date_t(current_ts)):
//                                                 BSON(key_processing_ageing << false <<
//                                                      key_last_checking_time << mongo::Date_t(current_ts))));
//
//        DEBUG_CODE(MDBNDA_ERR<<log_message("releaseNodeForAgeingManagement",
//                                           "update",
//                                           DATA_ACCESS_LOG_2_ENTRY("query",
//                                                                   "update",
//                                                                   query.jsonString(),
//                                                                   update.jsonString()));)
//        //find the control unit
//        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
//                                     query,
//                                     update))) {
//            MDBNDA_ERR << CHAOS_FORMAT("Error %1% fetching the next cheable control unit for ageing", %err);
//        }
//    } catch (const mongo::DBException &e) {
//        MDBNDA_ERR << e.what();
//        err = -1;
//    } catch (const CException &e) {
//        MDBNDA_ERR << e.what();
//        err = e.errorCode;
//    }
//    return err;
//}
