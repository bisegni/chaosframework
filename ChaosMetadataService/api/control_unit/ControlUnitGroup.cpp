/*
 *	ControlUnitGroup.cpp
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
#include "ControlUnitGroup.h"
#include "SetInstanceDescription.h"
#include "SearchInstancesByUS.h"
#include "GetInstance.h"
#include "DeleteInstance.h"
#include "InitDeinit.h"
#include "StartStop.h"

using namespace chaos::metadata_service::api::control_unit;
DEFINE_CLASS_FACTORY_NO_ALIAS(ControlUnitGroup, chaos::metadata_service::api::AbstractApiGroup);

ControlUnitGroup::ControlUnitGroup():
AbstractApiGroup("control_unit"){
        //add api for UnitServer registration
    addApi<SetInstanceDescription>();
    addApi<SearchInstancesByUS>();
    addApi<GetInstance>();
    addApi<DeleteInstance>();
    addApi<InitDeinit>();
    addApi<StartStop>();
}

ControlUnitGroup::~ControlUnitGroup() {

}