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
#include "NodeRegister.h"

#include <chaos/common/property/property.h>

#include "../../batch/unit_server/UnitServerAckBatchCommand.h"
#include "../../batch/control_unit/RegistrationAckBatchCommand.h"
#include "../../batch/agent/AgentAckCommand.h"
#include "../../batch/node/NodeAckCommand.h"

#include <boost/algorithm/string/predicate.hpp>

#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(NodeRegister, chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE);

CDWUniquePtr NodeRegister::execute(CDWUniquePtr api_data){
    CHECK_CDW_THROW_AND_LOG(api_data, USRA_ERR, -1, "No parameter found")
    CDWUniquePtr result;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        throw CException(-1, "Node unique id not found", __PRETTY_FUNCTION__);
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        throw CException(-2, "Node type not found", __PRETTY_FUNCTION__);
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_RPC_ADDR)) {
        throw CException(-2, "RPC address not found", __PRETTY_FUNCTION__);
    }
    
    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
        result = unitServerRegistration(MOVE(api_data));
    } else if(boost::starts_with(node_type, NodeType::NODE_TYPE_CONTROL_UNIT)) {
        result = controlUnitRegistration(MOVE(api_data));
    } else if(boost::starts_with(node_type, NodeType::NODE_TYPE_AGENT)) {
        result = agentRegistration(MOVE(api_data));
    } else {
        result = simpleRegistration(MOVE(api_data));
    } 
    /* else {
        throw CException(-3, "Type of node not managed for registration", __PRETTY_FUNCTION__);
    }*/
    return result;
}

CDWUniquePtr NodeRegister::agentRegistration(CDWUniquePtr api_data) {
    int err = 0;
    uint64_t    command_id;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //fetch the unit server data access
    GET_DATA_ACCESS(AgentDataAccess, a_da, -1)
    try {
        if((err = a_da->insertUpdateAgentDescription(*api_data))) {
            LOG_AND_TROW(USRA_ERR, -1, CHAOS_FORMAT("Error %1% registering agent %2%", %err%agent_uid));
        }
        //now we can send back the received message with the ack result
        api_data->addInt32Value(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
    } catch (...) {
        api_data->addInt32Value(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::agent::AgentAckCommand),
                                          api_data->clone().release());
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
    }
    
    //manage ack into back command
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::agent::AgentAckCommand),
                                                   api_data.release(),
                                                   0,
                                                   1000);
    
    USRA_INFO << CHAOS_FORMAT("Sent ack for agent %1% registration", %agent_uid);
    return CDWUniquePtr();
}
CDWUniquePtr NodeRegister::simpleRegistration(CDWUniquePtr api_data) {
    
    int err = 0;
    uint64_t    command_id;
    bool        alive = false;
    bool        is_present = false;
    uint64_t    nodes_seq = 0;
    
    //fetch the unit server data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -1);
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //check if node is already up and running
    if((err = n_da->isNodeAlive(node_uid, alive))){
        LOG_AND_TROW(USRA_ERR, -4, CHAOS_FORMAT("Error checking if node %1% already running", %node_uid));
    } else if(alive) {
        USRA_ERR<<"NODE:"<<node_uid<<" already alive wait 10s";
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INSTANCE_ALREADY_RUNNING);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::NodeAckCommand),
                                          api_data->clone().release());
        return CDWUniquePtr();
    }
    //we can porceed with uniserver registration
    USRA_INFO << "Registering NODE: " << node_uid;
    std::string ttype;
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -3)

    try {
        if((err = n_da->checkNodePresence(is_present,node_uid, ttype))) {
            //err
            api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                    ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
            LOG_AND_TROW(USRA_ERR, -3, "error checking the NODE presence")
        }
        if(is_present) {
            //present
            USRA_DBG<<"UPDATE EXISTING NODE:"<<node_uid << " type:"<<ttype;

            if((err = n_da->updateNode(*api_data.get()))) {
                api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                        ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -4, "error updating the NIDE information")
            }
        }else {
            USRA_DBG<<"ADDING NEW NODE:"<<node_uid;

            //unit server not found, so we need to register new one
            if((err = u_da->getNextSequenceValue("nodes", nodes_seq))) {
                LOG_AND_TROW(USRA_ERR, err, "error fetching the new sequence value")
            }else{
                //add new fetched sequecne
                api_data->addInt64Value("seq", nodes_seq);
                
                //insert th enew node
                if((err = n_da->insertNewNode(*api_data.get()))) {
                    api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                            ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    LOG_AND_TROW(USRA_ERR, -6, "error saving the NODE information")
                }
                if((err = n_da->updateNode(*api_data.get()))) {
                api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                        ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -4, "error updating the NIDE information")
            }
            }
        }
        //now we can send back the received message with the ack result
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
    } catch (chaos::CException& ex) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::NodeAckCommand),
                                                       api_data->clone().release());
        USRA_ERR << "Sent ack for registration denied to NODE " << node_uid;
        throw;
    } catch (...) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::NodeAckCommand),
                                          api_data->clone().release());
        USRA_ERR << "Sent ack for registration denied to NODE: " << node_uid;
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
    }
    
    //all is gone weel
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::NodeAckCommand),
                                                   api_data.release(),
                                                   0,
                                                   1000);
    
    USRA_INFO << "Sent ack for registration ok to the NODE " << node_uid;
    return NULL;
}

CDWUniquePtr NodeRegister::unitServerRegistration(CDWUniquePtr api_data) {
    
    int err = 0;
    uint64_t    command_id;
    bool        alive = false;
    bool        is_present = false;
    uint64_t    nodes_seq = 0;
    
    //fetch the unit server data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -1);
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -2)
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -3)
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //check if node is already up and running
    if((err = n_da->isNodeAlive(node_uid, alive))){
        LOG_AND_TROW(USRA_ERR, -4, CHAOS_FORMAT("Error checking if node %1% was runnign", %node_uid));
    } else if(alive) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INSTANCE_ALREADY_RUNNING);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                          api_data->clone().release());
        return CDWUniquePtr();
    }
    //we can porceed with uniserver registration
    const std::string unit_server_alias = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    USRA_INFO << "Register unit server " << unit_server_alias;
    try {
        if((err = us_da->checkPresence(unit_server_alias, is_present))) {
            //err
            api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                    ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
            LOG_AND_TROW(USRA_ERR, -3, "error checking the unit server presence")
        }if(is_present) {
            //present
            if((err = us_da->updateUS(*api_data))) {
                api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                        ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -4, "error updating the unit server information")
            }
        }else {
            //unit server not found, so we need to register new one
            if((err = u_da->getNextSequenceValue("nodes", nodes_seq))) {
                LOG_AND_TROW(USRA_ERR, err, "error fetching the new sequence value")
            }else{
                //add new fetched sequecne
                api_data->addInt64Value("seq", nodes_seq);
                
                //insert th enew node
                if((err = us_da->insertNewUS(*api_data))) {
                    api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                            ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    LOG_AND_TROW(USRA_ERR, -6, "error saving the new unit server information")
                }
                if((err = us_da->insertNewUS(*api_data))) {
                    api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                            ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    LOG_AND_TROW(USRA_ERR, -6, "error saving the new unit server information")
                }
            }
        }
        //now we can send back the received message with the ack result
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
    } catch (chaos::CException& ex) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                                       api_data->clone().release());
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
        throw;
    } catch (...) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                          api_data->clone().release());
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
    }
    
    //all is gone weel
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                                   api_data.release(),
                                                   0,
                                                   1000);
    
    USRA_INFO << "Sent ack for registration ok to the unit server " << unit_server_alias;
    return NULL;
}

//! perform specific registration for control unit
CDWUniquePtr NodeRegister::controlUnitRegistration(CDWUniquePtr api_data) {
    int         err = 0;
    uint64_t    command_id;
    std::string us_host;
    bool        is_present = false;
    bool        loaded_from_unit_server = false;
    bool        has_an_unit_server = false;
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_RPC_DOMAIN, USRA_ERR, -1, "The rpc domain key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, USRA_ERR, -2, "The cudk_ds_desc key is mandatory")
    
    //fetch the unit server data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -5)
    
    //allocate datapack for batch command
    ChaosUniquePtr<chaos::common::data::CDataWrapper> ack_command(new CDataWrapper());
    const std::string cu_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    USRA_INFO << "Register control unit " << cu_uid;
    
    //set cu id to the batch command datapack
    ack_command->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                                cu_uid);
    //aset rpc address
    ack_command->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR,
                                api_data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR));
    //aset rpc address
    ack_command->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN,
                                api_data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN));
    
    try {
        //check if the cu has been loaded from unit server
        if(api_data->hasKey("mds_control_key")) {
            const std::string mds_ctrl_key = api_data->getStringValue("mds_control_key");
            loaded_from_unit_server = (mds_ctrl_key.compare("mds") == 0);
        }
        
        //check if the cu has a parent
        //we need to check if the control unit is assocaite to an unit server
        if((err = us_da->getUnitserverForControlUnitID(cu_uid,
                                                       us_host))){
            LOG_AND_TROW_FORMATTED(USRA_ERR, -6, "Error searching unit server for control unit %1% with code %2%",%cu_uid%err);
        }
        has_an_unit_server = (us_host.size()>0);
        
        if(has_an_unit_server) {
            //@TODO removed this check that is wrong for WAN dataset registration
            //  if(loaded_from_unit_server == false) {LOG_AND_TROW_FORMATTED(USRA_ERR, -7, "The control unit %1% need to be loaded from the unit server %2%",%cu_uid%us_host);}
        }
        
        //check if the node is present
        if((err = cu_da->checkPresence(cu_uid, is_present))) {
            LOG_AND_TROW(USRA_ERR, -8, "error checking the control unit presence");
        } if (is_present) {
            if((err = n_da->updateNode(*api_data))) {
                LOG_AND_TROW(USRA_ERR, err, "Error updating default node field");
            }
            
            if((err = cu_da->setDataset(cu_uid,
                                        *api_data))){
                LOG_AND_TROW(USRA_ERR, err, "error setting the dataset of the control unit");
            }
            //ok->registered
            ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                       ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
        } else {
            
            USRA_INFO << "Control unit " << cu_uid << " not present, check if it is hosted by an unit server";
            //we need to check if the control unit is assocaite to an unit server
            if((err = us_da->getUnitserverForControlUnitID(cu_uid,
                                                           us_host) == 0)){
                if(us_host.size() == 0) {
                    USRA_INFO << "Control unit " << cu_uid << " doesn't belong to any unit server so can ste forwardar";
                    
                    if((err = cu_da->insertNewControlUnit(*api_data))) {
                        LOG_AND_TROW_FORMATTED(USRA_ERR, err, "Error creating new control unit %1%",%cu_uid);
                    }
                    
                    if((err = cu_da->setDataset(cu_uid,
                                                *api_data))){
                        LOG_AND_TROW_FORMATTED(USRA_ERR, err, "error setting the dataset for the control unit %1%",%cu_uid);
                    }
                }else {
                    if(loaded_from_unit_server == false) {
                        //need to be stoped becaus the cu need be loaded form unit server
                        USRA_ERR << "The control unit "<< cu_uid <<" need to be stoped because the cu need be loaded form unit server";
                        //actiually only the control unit hosted by unit server can be registered
                        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    }
                }
            } else {
                LOG_AND_TROW_FORMATTED(USRA_ERR, -9, "Error searchin unit server for control unit %1% with code %2%",%cu_uid%err);
            }
            
        }
        
        //!save porperty
        PropertyGroupVectorSDWrapper pgv_swd;
        pgv_swd.serialization_key = "property";
        pgv_swd.deserialize(api_data.get());
        if((err = n_da->setProperty(cu_uid, pgv_swd()))) {
            LOG_AND_TROW_FORMATTED(USRA_ERR, err, "Error on node porperty update for %1%",%cu_uid);
        }
        
        //set the code to inform cu that all is gone well
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command->clone().release(),
                                                       0,
                                                       1000);
        USRA_INFO << "Sent ack for registration ok to the control unit " << cu_uid << " with commadn id:" <<command_id;
    } catch (chaos::CException& ex) {
        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        
        USRA_ERR << "Sent ack for registration denied to the unit server " << cu_uid;
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command->clone().release());
        throw;
    } catch (...) {
        USRA_ERR << "Sent ack for registration denied to the unit server " << cu_uid;
        //somenthing goes worng so deny the registration
        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command->clone().release());
        LOG_AND_TROW(USRA_ERR, -6, "Unknown exception")
    }
    return CDWUniquePtr();
}

