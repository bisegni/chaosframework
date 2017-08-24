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
