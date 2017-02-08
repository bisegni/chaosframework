/*
 *	ListUnitServerForAgent.cpp
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

#include "ListUnitServerForAgent.h"

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(ListUnitServerForAgent)
#define ERR  DBG_LOG(ListUnitServerForAgent)
#define DBG  ERR_LOG(ListUnitServerForAgent)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

ListUnitServerForAgent::ListUnitServerForAgent():
AbstractApi("listUnitServerForAgent"){
}

ListUnitServerForAgent::~ListUnitServerForAgent() {
}

CDataWrapper *ListUnitServerForAgent::execute(CDataWrapper *api_data, bool& detach_data) {
    return NULL;
}
