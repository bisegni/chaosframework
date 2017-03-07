/*
 *	LogWorker.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos_service_common/data/node/Agent.h>

#include "LogWorker.h"

#define INFO  INFO_LOG(LogWorker)
#define ERROR ERR_LOG(LogWorker)
#define DBG   DBG_LOG(LogWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::data;
using namespace chaos::common::utility;

LogWorker::LogWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN) {
    //register rpc action
    DeclareAction::addActionDescritionInstance<LogWorker>(this,
                                                          &LogWorker::starLoggingAssociation,
                                                          getName(),
                                                          AgentNodeDomainAndActionRPC::LogWorker::ACTION_START_LOGGING_ASSOCIATION,
                                                          "Start logging a managed process consol output");
    DeclareAction::addActionDescritionInstance<LogWorker>(this,
                                                          &LogWorker::stopLoggingAssociation,
                                                          getName(),
                                                          AgentNodeDomainAndActionRPC::LogWorker::ACTION_STOP_LOGGING_ASSOCIATION,
                                                          "Start logging a managed process consol output");
    
}

LogWorker::~LogWorker() {}

void LogWorker::init(void *data) throw(chaos::CException) {}

void LogWorker::deinit() throw(chaos::CException) {}

CDataWrapper *LogWorker::starLoggingAssociation(CDataWrapper *data,
                                                bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Input data is mandatory");
    CHECK_MANDATORY_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERROR, -2);
    CHECK_TYPE_OF_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, CDataWrapper, ERROR, -3);

    std::auto_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper node_association;
    node_association.deserialize(assoc_ser.get());
    
    return NULL;
}

CDataWrapper *LogWorker::stopLoggingAssociation(CDataWrapper *data,
                                                bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Input data is mandatory");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERROR, -2);
    CHECK_TYPE_OF_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, CDataWrapper, ERROR, -3);
    
    std::auto_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper node_association;
    node_association.deserialize(assoc_ser.get());
    return NULL;
}
