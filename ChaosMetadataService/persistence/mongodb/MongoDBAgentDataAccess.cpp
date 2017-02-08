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
        CHECK_KEY_THROW_AND_LOG((&agent_description), AgentNodeDefinitionKey::HOSTED_WORKER, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %AgentNodeDefinitionKey::HOSTED_WORKER));
        
        //now update proprietary fields
        const std::string agent_uid = agent_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        
        //!check if node is present
        if((err = node_data_access->checkNodePresence(presence, agent_uid))) {
            ERR << CHAOS_FORMAT("Error registering agent %1% with error %2%" , %agent_uid%err);
            return err;
        }
        
        if(presence == false) {
            //create new empty node
            CDataWrapper new_node;
            new_node.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
            new_node.addStringValue(NodeDefinitionKey::NODE_TYPE,  NodeType::NODE_TYPE_AGENT);
            node_data_access->insertNewNode(new_node);
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
        
        mongo::BSONObj update = BSON("$set" << BSON(AgentNodeDefinitionKey::HOSTED_WORKER << array_descirption_builder.arr()));
        
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

int MongoDBAgentDataAccess::getNodeListForAgent(const std::string& agent_uid,
                                                ChaosStringVector& unist_server_associated) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        mongo::BSONObj project = BSON(CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << 1);
        
        unist_server_associated.clear();
        DEBUG_CODE(DBG<<log_message("getNodeListForAgent",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &project))){
            ERR << CHAOS_FORMAT("Error finding associated nodes to agent %1% with error %2%", %agent_uid%err);
        } else if(result.isEmpty() == false) {
            mongo::BSONElement ele = result.getField(AgentNodeDefinitionKey::NODE_ASSOCIATED);
            if(ele.type() == mongo::Array) {
                std::vector<mongo::BSONElement> associated_node_object = ele.Array();
                for(std::vector<mongo::BSONElement>::iterator it = associated_node_object.begin(),
                    end = associated_node_object.end();
                    it != end;
                    it++) {
                    mongo::BSONObj attribute_config = it->Obj();
                    unist_server_associated.push_back(attribute_config.getField(NodeDefinitionKey::NODE_UNIQUE_ID));
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

int MongoDBAgentDataAccess::saveNodeAssociationForAgent(const std::string& agent_uid,
                                                        AgentAssociation& unist_server_association) {
    int err = 0;
    try {
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT
                                    << CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << unist_server_association.associated_node_uid);
        
        
        
        mongo::BSONObj update = BSON("$set" << BSON(CHAOS_FORMAT("%1%$%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_CFG) <<  unist_server_association.configuration_file_content <<
                                                    CHAOS_FORMAT("%1%$%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_AUTO_START)<< unist_server_association.auto_start));
        
        DEBUG_CODE(DBG<<log_message("saveNodeAssociationForAgent",
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
            ERR << CHAOS_FORMAT("Error saving association of %3% into the agent %1% with error %2%", %agent_uid%err%unist_server_association.associated_node_uid);
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
                                                        AgentAssociation& unist_server_association) {
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT
                                    << CHAOS_FORMAT("%1%.%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%NodeDefinitionKey::NODE_UNIQUE_ID) << associated_node_uid);
        
        
        
        mongo::BSONObj update = BSON("$set" << BSON(CHAOS_FORMAT("%1%$%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_CFG) <<  unist_server_association.configuration_file_content <<
                                                    CHAOS_FORMAT("%1%$%2%",%AgentNodeDefinitionKey::NODE_ASSOCIATED%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_AUTO_START)<< unist_server_association.auto_start));
        
        DEBUG_CODE(DBG<<log_message("loadNodeAssociationForAgent",
                                    "find",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            query.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query))){
            ERR << CHAOS_FORMAT("Error loading the association of %3% into the agent %1% with error %2%", %agent_uid%err%associated_node_uid);
        } else if(result.isEmpty() == false) {
            mongo::BSONElement ele = result.getField(AgentNodeDefinitionKey::NODE_ASSOCIATED);
            if(ele.type() == mongo::Array) {
                std::vector<mongo::BSONElement> associated_node_object = ele.Array();
                if(associated_node_object.size()>0) {
                    mongo::BSONObj associate_node_configuration = associated_node_object[0].Obj();
                    //we have found description and need to return all field
                    std::auto_ptr<CDataWrapper> associ_cfg_wrap(new CDataWrapper(associate_node_configuration.objdata()));
                    AgentAssociationSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, unist_server_association));
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
