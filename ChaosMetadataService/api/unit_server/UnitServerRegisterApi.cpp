/*
 *	UnitServerRegisterApi.cpp
 *	!CHOAS
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
#include "UnitServerRegisterApi.h"

#define USRA_INFO INFO_LOG(MongoDBUnitServerDataAccess)
#define USRA_DBG  INFO_DBG(MongoDBUnitServerDataAccess)
#define USRA_ERR  INFO_ERR(MongoDBUnitServerDataAccess)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;

UnitServerRegisterApi::UnitServerRegisterApi():
AbstractApi("registerUnitServer"){
    
}

UnitServerRegisterApi::~UnitServerRegisterApi() {
    
}

chaos::common::data::CDataWrapper *UnitServerRegisterApi::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    USRA_INFO << api_data->getJSONString();
    getPersistenceDriver()->getProducerDataAccess();
    return NULL;
}