/*
 *	NodeApi.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "NodeGroup.h"
#include "NodeRegister.h"
#include "NodeCreateUnitServer.h"
#include "NodeSearch.h"
#include "NodeGetDescription.h"
#include "ChangeNodeState.h"
#include "CommandGet.h"
#include "CommandTemplateSet.h"
#include "CommandTemplateGet.h"
#include "CommandTemplateDelete.h"
#include "CommandTemplateSearch.h"
#include "CommandTemplateSubmit.h"
#include "CommandInstanceSubmit.h"

using namespace chaos::metadata_service::api::node;
DEFINE_CLASS_FACTORY_NO_ALIAS(NodeGroup, chaos::metadata_service::api::AbstractApiGroup);

NodeGroup::NodeGroup():
AbstractApiGroup("system"){
    //add api for UnitServer registration
    addApi<NodeSearch>();
    addApi<NodeRegister>();
    addApi<NodeCreateUnitServer>();
    addApi<NodeGetDescription>();
    addApi<ChangeNodeState>();
    addApi<CommandGet>();
    addApi<CommandTemplateSet>();
    addApi<CommandTemplateGet>();
    addApi<CommandTemplateDelete>();
    addApi<CommandTemplateSearch>();
    addApi<CommandTemplateSubmit>();
    addApi<CommandInstanceSubmit>();
}

NodeGroup::~NodeGroup() {}