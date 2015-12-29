/*
 *	GroupsApiGroup.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include "GroupsApiGroup.h"
#include "AddNode.h"
#include "GetNodeChilds.h"
#include "GetDomains.h"
#include "DeleteNode.h"
using namespace chaos::metadata_service::api::groups;

DEFINE_CLASS_FACTORY_NO_ALIAS(GroupsApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

GroupsApiGroup::GroupsApiGroup():
AbstractApiGroup("groups"){
    addApi<AddNode>();
    addApi<DeleteNode>();
    addApi<GetDomains>();
    addApi<GetNodeChilds>();
}

GroupsApiGroup::~GroupsApiGroup() {}
