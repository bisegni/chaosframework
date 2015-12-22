/*
 *	UnitServerApiGroup.cpp
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

#include "UnitServerApiGroup.h"
#include "GetDescription.h"
#include "LoadUnloadControlUnit.h"
#include "NewUS.h"
#include "ManageCUType.h"
#include "DeleteUS.h"
using namespace chaos::metadata_service::api::unit_server;
DEFINE_CLASS_FACTORY_NO_ALIAS(UnitServerApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

UnitServerApiGroup::UnitServerApiGroup():
AbstractApiGroup(UnitServerNodeDomainAndActionRPC::RPC_DOMAIN){
	//add api for producer registration
	addApi<GetDescription>();
    addApi<LoadUnloadControlUnit>();
    addApi<NewUS>();
    addApi<DeleteUS>();
    addApi<ManageCUType>();
}

UnitServerApiGroup::~UnitServerApiGroup() {
	
}