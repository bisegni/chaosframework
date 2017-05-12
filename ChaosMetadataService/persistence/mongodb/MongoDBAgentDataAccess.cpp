/*
 *	MongoDBAgentDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#include "MongoDBAgentDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::data::agent;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO    INFO_LOG(MongoDBScriptDataAccess)
#define ERR     DBG_LOG(MongoDBScriptDataAccess)
#define DBG     ERR_LOG(MongoDBScriptDataAccess)

MongoDBAgentDataAccess::MongoDBAgentDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
AgentDataAccess(),
utility_data_access(NULL),
node_data_access(NULL){}

MongoDBAgentDataAccess::~MongoDBAgentDataAccess() {}


int MongoDBAgentDataAccess::insertUpdateAgentDescription(CDataWrapper& agent_description) {
    int err = 0;
    int size = 0;
    bool presence = false;
    try {
        CHECK_KEY_THROW_AND_LOG((&agent_description), NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %NodeDefinitionKey::NODE_UNIQUE_ID));
        CHECK_KEY_THROW_AND_LOG((&agent_description), NodeDefinitionKey::NODE_RPC_ADDR, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %NodeDefinitionKey::NODE_RPC_ADDR));
        CHECK_KEY_THROW_AND_LOG((&agent_description), AgentNodeDefinitionKey::HOSTED_WORKER, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %AgentNodeDefinitionKey::HOSTED_WORKER));
        CHECK_KEY_THROW_AND_LOG((&agent_description), AgentNodeDefinitionKey::WORKING_DIRECTORY, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %AgentNodeDefinitionKey::WORKING_DIRECTORY));
        
        //now update proprietary fields
        const std::string agent_uid = agent_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        
        //!check if node is present
        if((err = node_data_access->checkNodePresence(presence, agent_uid))) {
            ERR << CHAOS_FORMAT("Error checking agent %1% presence with error %2%" , %agent_uid%err);
            return err;
        }
        
        if(presence == false) {
            //create new empty node
            CDataWrapper new_node;
            new_node.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
            new_node.addStringValue(NodeDefinitionKey::NODE_TYPE,  NodeType::NODE_TYPE_AGENT);
            if((err = node_data_access->insertNewNode(new_node))) {
                ERR << CHAOS_FORMAT("Error creating a new node structure for agent %1% with error %2%" , %agent_uid%err);
                return err;
            } else if((err = node_data_access->addAgeingManagementDataToNode(agent_uid))) {
                ERR << CHAOS_FORMAT("Error adding ageing structure to agent %1% with error %2%" , %agent_uid%err);
                return err;
            }
        }
        
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        mongo::BSONArrayBuilder array_descirption_builder;
        std::auto_ptr<CMultiTypeDataArrayWrapper> description_array(agent_description.getVectorValue(AgentNodeDefinitionKey::HOSTED_WORKER));
        for(int idx = 0;
            idx < description_array->size();
            idx++) {
            std::auto_ptr<CDataWrapper> worker_desc(description_array->getCDataWrapperElementAtIndex(idx));
            array_descirption_builder << mongo::BSONObj(worker_desc->getBSONRawData(size));
        }
        
        mongo::BSONObj update = BSON("$set" << BSON(NodeDefinitionKey::NODE_RPC_ADDR << agent_description.getStringValue(NodeDefinitionKey::NODE_RPC_ADDR) <<
                                                    AgentNodeDefinitionKey::WORKING_DIRECTORY << agent_description.getStringValue(AgentNodeDefinitionKey::WORKING_DIRECTORY) <<
                                                    AgentNodeDefinitionKey::HOSTED_WORKER << array_descirption_builder.arr()));
        
        DEBUG_CODE(DBG<<log_message("insertUpdateAgentDescription",
                                    "update",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Update",
                                                            query.toString(),
                                                            update.toString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update,
                                     true,
                                     false))){
            ERR << "Error registering agent" << agent_uid << " with error:" << err;
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::loadAgentDescription(const std::string& agent_uid,
                                                 const bool load_related_data,
                                                 AgentInstance& agent_description) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        mongo::BSONObj project = (load_related_data?BSON("_id"<<-1):BSON("_id" <<-1 << AgentNodeDefinitionKey::NODE_ASSOCIATED<<-1));
        DEBUG_CODE(DBG<<log_message("loadAgentDescription",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query))){
            ERR << CHAOS_FORMAT("Error finding agent %1% with error %2%", %agent_uid%err);
        } else if(result.isEmpty() == false &&
                  result.hasField(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
            std::auto_ptr<CDataWrapper> full_ser(new CDataWrapper(result.objdata()));
            AgentInstanceSDWrapper agent_instance_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentInstance, agent_description));
            agent_instance_sd_wrapper.deserialize(full_ser.get());
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::getAgentForNode(const std::string& associated_node_uid,
                                            std::string& agent_uid) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT <<
                                    CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << associated_node_uid);
        mongo::BSONObj project = BSON( NodeDefinitionKey::NODE_UNIQUE_ID << 1);
        
        DEBUG_CODE(DBG<<log_message("getAgentForNode",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &project))){
            ERR << CHAOS_FORMAT("Error searching agent for node %1% with error %2%", %associated_node_uid%err);
        } else if(result.isEmpty() == false &&
                  result.hasField(NodeDefinitionKey::NODE_UNIQUE_ID)) {
            agent_uid = result.getField(NodeDefinitionKey::NODE_UNIQUE_ID).String();
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::saveNodeAssociationForAgent(const std::string& agent_uid,
                                                        AgentAssociation& node_association) {
    int err = 0;
    int size = 0;
    AgentAssociation current_association;
    try {
        //!fetch current assocaition
        if((err = loadNodeAssociationForAgent(agent_uid,
                                              node_association.associated_node_uid,
                                              current_association))) {
            LOG_AND_TROW(ERR, -1, CHAOS_FORMAT("Error loading association of %3% into the agent %1% with error %2%", %agent_uid%err%node_association.associated_node_uid));
        } else {
            if(current_association.association_unique_id.size() != 0) {
                node_association.association_unique_id = current_association.association_unique_id;
            } else {
                node_association.association_unique_id = UUIDUtil::generateUUIDLite();
            }
        }
        
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        AgentAssociationSDWrapper assoc_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, node_association));
        std::auto_ptr<CDataWrapper> assoc_ser = assoc_wrap.serialize();
        mongo::BSONObj pull_update = BSON("$pull" << BSON(AgentNodeDefinitionKey::NODE_ASSOCIATED << BSON(NodeDefinitionKey::NODE_UNIQUE_ID << node_association.associated_node_uid)));
        mongo::BSONObj push_update = BSON("$push" << BSON(AgentNodeDefinitionKey::NODE_ASSOCIATED << mongo::BSONObj(assoc_ser->getBSONRawData(size))));
        
        DEBUG_CODE(DBG<<log_message("saveNodeAssociationForAgent",
                                    "pull",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Pull",
                                                            query.toString(),
                                                            pull_update.toString()));)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     pull_update))){
            ERR << CHAOS_FORMAT("Error pulling association of %3% into the agent %1% with error %2%", %agent_uid%err%node_association.associated_node_uid);
        }
        
        DEBUG_CODE(DBG<<log_message("saveNodeAssociationForAgent",
                                    "push",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Push",
                                                            query.toString(),
                                                            push_update.toString()));)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     push_update))){
            ERR << CHAOS_FORMAT("Error pushing association of %3% into the agent %1% with error %2%", %agent_uid%err%node_association.associated_node_uid);
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::loadNodeAssociationForAgent(const std::string& agent_uid,
                                                        const std::string& associated_node_uid,
                                                        AgentAssociation& node_association) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT
                                    << CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << associated_node_uid);
        mongo::BSONObj projection = BSON(AgentNodeDefinitionKey::NODE_ASSOCIATED << BSON("$elemMatch" << BSON(NodeDefinitionKey::NODE_UNIQUE_ID << associated_node_uid)));
        
        
        DEBUG_CODE(DBG<<log_message("loadNodeAssociationForAgent",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &projection))){
            ERR << CHAOS_FORMAT("Error loading the association of %3% into the agent %1% with error %2%", %agent_uid%err%associated_node_uid);
        } else if(result.isEmpty() == false) {
            mongo::BSONElement ele = result.getField(AgentNodeDefinitionKey::NODE_ASSOCIATED);
            if(ele.type() == mongo::Array) {
                std::vector<mongo::BSONElement> associated_node_object = ele.Array();
                if(associated_node_object.size()>0) {
                    mongo::BSONObj associate_node_configuration = associated_node_object[0].Obj();
                    //we have found description and need to return all field
                    std::auto_ptr<CDataWrapper> associ_cfg_wrap(new CDataWrapper(associate_node_configuration.objdata()));
                    AgentAssociationSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, node_association));
                    sd_wrap.deserialize(associ_cfg_wrap.get());
                }
            }
            
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::removeNodeAssociationForAgent(const std::string& agent_uid,
                                                          const std::string& associated_node_uid) {
    int err = 0;
    try {
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        
        mongo::BSONObj pull_update = BSON("$pull" << BSON(AgentNodeDefinitionKey::NODE_ASSOCIATED << BSON(NodeDefinitionKey::NODE_UNIQUE_ID << associated_node_uid)));
        
        DEBUG_CODE(DBG<<log_message("removeNodeAssociationForAgent",
                                    "pull",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Pull",
                                                            query.toString(),
                                                            pull_update.toString()));)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     pull_update))){
            ERR << CHAOS_FORMAT("Error pulling association of %3% into the agent %1% with error %2%", %agent_uid%err%associated_node_uid);
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::setNodeAssociationStatus(const std::string& agent_uid,
                                                     const chaos::service_common::data::agent::AgentAssociationStatus& status) {
    int err = 0;
    try {
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT
                                    << CHAOS_FORMAT("%1%.%2%", %AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << status.associated_node_uid);
        
        mongo::BSONObj update = BSON("$set" << BSON(CHAOS_FORMAT("%1%.$.%2%", %AgentNodeDefinitionKey::NODE_ASSOCIATED%"alive") << status.alive <<
                                                    CHAOS_FORMAT("%1%.$.%2%", %AgentNodeDefinitionKey::NODE_ASSOCIATED%"check_ts") << mongo::Date_t(TimingUtil::getTimeStamp())));
        
        DEBUG_CODE(DBG<<log_message("setNodeAssociationStatus",
                                    "update",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "update",
                                                            query.toString(),
                                                            update.toString()));)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))){
            ERR << CHAOS_FORMAT("Error setting alive information for node %3% into the agent %1% with error %2%", %agent_uid%err%status.associated_node_uid);
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::getNodeListStatusForAgent(const std::string& agent_uid,
                                                      VectorAgentAssociationStatus& node_status_vec) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        mongo::BSONObj project = BSON(CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << 1 <<
                                      CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%"alive") << 1 <<
                                      CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%"check_ts") << 1);
        
        node_status_vec.clear();
        DEBUG_CODE(DBG<<log_message("getNodeListForAgent",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &project))){
            ERR << CHAOS_FORMAT("Error finding associated nodes to agent %1% with error %2%", %agent_uid%err);
        } else if(result.isEmpty() == false &&
                  result.hasField(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
            mongo::BSONElement ele = result.getField(AgentNodeDefinitionKey::NODE_ASSOCIATED);
            if(ele.type() == mongo::Array) {
                std::vector<mongo::BSONElement> associated_node_object = ele.Array();
                for(std::vector<mongo::BSONElement>::iterator it = associated_node_object.begin(),
                    end = associated_node_object.end();
                    it != end;
                    it++) {
                    mongo::BSONObj obj = it->Obj();
                    AgentAssociationStatusSDWrapper status_sd_wrapper;
                    std::auto_ptr<CDataWrapper> status(new CDataWrapper(obj.objdata()));
                    status_sd_wrapper.deserialize(status.get());
                    if(obj.hasField("check_ts")){status_sd_wrapper().check_ts = obj.getField("check_ts").date().millis;}
                    node_status_vec.push_back(status_sd_wrapper());
                }
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::pushLogEntry(const std::string& node_uid,
                                         const std::string& node_log_entry) {
    int err = 0;
    try {
        mongo::BSONObj result;
        uint64_t cur_ts = TimingUtil::getTimeStampInMicroseconds();
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << node_uid
                                    << MONGO_DB_FIELD_AGENT_PROCESS_LOG_TS << mongo::Date_t(cur_ts/1000)
                                    << MONGO_DB_FIELD_AGENT_PROCESS_LOG_SEQ << (long long)cur_ts
                                    << MONGO_DB_FIELD_AGENT_PROCESS_LOG_ENTRY << node_log_entry);
        
        
        DEBUG_CODE(DBG<<log_message("pushLogEntry",
                                    "inser",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_AGENT_PROCESS_LOG),
                                     query))){
            ERR << CHAOS_FORMAT("Error creating new elog entry for node %1% with error %2%", %node_uid%err);
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::getLogEntry(const std::string& node_uid,
                                        const int32_t number_of_entries,
                                        const bool asc,
                                        const uint64_t start_seq,
                                        VectorAgentLogEntry& found_entries) {
    int err = 0;
    try {
        found_entries.clear();
        std::vector<mongo::BSONObj> result;
        mongo::BSONObjBuilder query_builder;
        query_builder << NodeDefinitionKey::NODE_UNIQUE_ID << node_uid;
        if(start_seq) {
            if(asc) {
                query_builder << "log_seq" << BSON("$gt" << (long long)start_seq);
            } else {
                query_builder << "log_seq" << BSON("$lt" << (long long)start_seq);
            }
        }
        mongo::Query query = query_builder.obj();
        query = query.sort(BSON(MONGO_DB_FIELD_AGENT_PROCESS_LOG_TS<<(asc?1:-1)
                                <<"log_seq"<<(asc?1:-1)));
        DEBUG_CODE(DBG<<log_message("getLogEntry",
                                    "findN",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        connection->findN(result,
                          MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_AGENT_PROCESS_LOG),
                          query,
                          number_of_entries);
        if(result.size()) {
            for(std::vector<mongo::BSONObj>::iterator it = result.begin(),
                end = result.end();
                it != end;
                it++) {
                AgentLogEntry e;
                e.entry_ts = (uint64_t)(it->hasField("log_ts")?it->getField("log_ts").date().asInt64():0);
                e.entry_seq = (uint64_t)(it->hasField("log_ts")?it->getField("log_seq").date().asInt64():0);
                e.entry_log_line = (it->hasField("log_entry")?it->getField("log_entry").String():"");
                found_entries.push_back(e);
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::setLogEntryExpiration(const bool expiration_active,
                                                  const uint32_t expiration_in_seconds) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj index_key =  BSON(MONGO_DB_FIELD_AGENT_PROCESS_LOG_TS << 1);
        if((err = connection->dropIndex(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_AGENT_PROCESS_LOG),
                                        "agent_log_ttl_index"))) {
            ERR << CHAOS_FORMAT("Error dropping for ttl index with code %1%",%err);
        } else if(expiration_active) {
            if((err = connection->ensureIndex(getDatabaseName(),
                                              MONGO_DB_COLLECTION_AGENT_PROCESS_LOG,
                                              index_key,
                                              false,
                                              "agent_log_ttl_index",
                                              false,
                                              true,
                                              1,
                                              expiration_in_seconds))){
                ERR << CHAOS_FORMAT("Error creating new log ttl index with error %1%",%err);
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBAgentDataAccess::getLogEntryExpiration(bool& expiration_active,
                                                  uint32_t& expiration_in_seconds) {
    int err = 0;
    try {
        mongo::BSONObj r;
        if((err = connection->getIndex(r,
                                       MONGO_DB_COLLECTION_AGENT_PROCESS_LOG,
                                       "agent_log_ttl_index"))) {
            
        } else if(r.isEmpty() == true) {
            ERR << "No expiration found";
            expiration_active = false;
            expiration_in_seconds = 0;
        } else {
            DBG << r.toString();
            if(r.hasField("expireAfterSeconds") == false) {
                expiration_active = false;
                expiration_in_seconds = 0;
            }
            mongo::BSONElement e = r.getField("expireAfterSeconds");
            if(e.type() == mongo::NumberInt) {
                expiration_active = true;
                expiration_in_seconds = (uint32_t)e.numberInt();
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}
