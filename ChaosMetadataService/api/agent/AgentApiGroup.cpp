/*
 *	AgentApiGroup.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/02/2017 INFN, National Institute of Nuclear Physics
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

#include "AgentApiGroup.h"
#include "ListNodeForAgent.h"
#include "LoadNodeAssociation.h"
#include "SaveNodeAssociation.h"
#include "RemoveNodeAssociation.h"
#include "NodeOperation.h"
#include "LoadAgentDescription.h"
#include "CheckAgentHostedProcess.h"
#include "GetManagementConfiguration.h"
#include "SetManagementConfiguration.h"

using namespace chaos::metadata_service::api::agent;

DEFINE_CLASS_FACTORY_NO_ALIAS(AgentApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

AgentApiGroup::AgentApiGroup():
AbstractApiGroup(AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN){
    addApi<ListNodeForAgent>();
    addApi<LoadNodeAssociation>();
    addApi<SaveNodeAssociation>();
    addApi<RemoveNodeAssociation>();
    addApi<NodeOperation>();
    addApi<LoadAgentDescription>();
    addApi<CheckAgentHostedProcess>();
    addApi<GetManagementConfiguration>();
    addApi<SetManagementConfiguration>();
}

AgentApiGroup::~AgentApiGroup() {}
