/*
 *	GetSetFullUnitServer.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2017 INFN, National Institute of Nuclear Physics
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

#include "GetSetFullUnitServer.h"
#include "NewUS.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;
using namespace chaos::metadata_service::persistence::data_access;

#define US_ACT_INFO INFO_LOG(GetSetFullUnitServer)
#define US_ACT_DBG  DBG_LOG(GetSetFullUnitServer)
#define US_ACT_ERR  ERR_LOG(GetSetFullUnitServer)

GetSetFullUnitServer::GetSetFullUnitServer():
AbstractApi("GetSetFullUnitServer"){
    
}

GetSetFullUnitServer::~GetSetFullUnitServer() {
    
}

CDataWrapper *GetSetFullUnitServer::execute(CDataWrapper *api_data,
                             bool& detach_data) throw(chaos::CException) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, US_ACT_ERR, -1, "No parameter has been set!")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, US_ACT_ERR, -2, "No ndk_uid is mandatory!")
    
    int err = 0;
    bool presence = false;
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -5)
    //get the parameter
    const std::string us_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);

    bool setop=api_data->hasKey("fulldesc")&&api_data->hasKey("operation");

    if((err = us_da->checkPresence(us_uid, presence))) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -6, "Error fetch the presence for the uid:%1%", % us_uid);
    }
    
    //UnitServer is not there in a get operation
    if(!presence && !setop) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -7, "The unit server '%1%' has not been found", % us_uid);
    }
    if(setop){
    	if(presence){
    		if(api_data->getBoolValue("operation")){
    		    		 if((err = us_da->deleteUS(us_uid))){
    		    		            LOG_AND_TROW(US_ACT_ERR, -6, boost::str(boost::format("Error removing unit server of id:%1%") % us_uid));
    		    		 }
    		}
    	} else {
    		// create the entry
    		NewUS creat;
    		creat.execute(api_data,detach_data);
    	}


    }

    return NULL;
}
