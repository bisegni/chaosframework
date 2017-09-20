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
#include "NodeGroup.h"
#include "NodeRegister.h"
#include "NodeLoadCompletion.h"
#include "NodeCreateUnitServer.h"
#include "NodeSearch.h"
#include "NodeHealthStatus.h"
#include "NodeGetDescription.h"
#include "ChangeNodeState.h"
#include "CommandGet.h"
#include "UpdateProperty.h"
#include "UpdatePropertyDefaultValues.h"
#include "GetPropertyDefaultValues.h"
#include "CommandTemplateSet.h"
#include "CommandTemplateGet.h"
#include "CommandTemplateDelete.h"
#include "CommandTemplateSearch.h"
#include "CommandTemplateSubmit.h"
#include "CommandInstanceSubmit.h"
#include "GetCommandAndTemplateDescription.h"

using namespace chaos::metadata_service::api::node;
DEFINE_CLASS_FACTORY_NO_ALIAS(NodeGroup, chaos::metadata_service::api::AbstractApiGroup);

NodeGroup::NodeGroup():
AbstractApiGroup(NodeDomainAndActionRPC::RPC_DOMAIN){
    //add api for UnitServer registration
    addApi<NodeSearch>();
    addApi<NodeRegister>();
    addApi<CommandGet>();
    addApi<UpdateProperty>();
    addApi<ChangeNodeState>();
    addApi<NodeHealthStatus>();
    addApi<CommandTemplateSet>();
    addApi<CommandTemplateGet>();
    addApi<NodeLoadCompletion>();
    addApi<NodeGetDescription>();
    addApi<NodeCreateUnitServer>();
    addApi<CommandTemplateDelete>();
    addApi<CommandTemplateSearch>();
    addApi<CommandTemplateSubmit>();
    addApi<CommandInstanceSubmit>();
    addApi<GetPropertyDefaultValues>();
    addApi<UpdatePropertyDefaultValues>();
    addApi<GetCommandAndTemplateDescription>();
}

NodeGroup::~NodeGroup() {}
