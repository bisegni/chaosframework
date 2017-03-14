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
#include "../control_unit/SetInstanceDescription.h"
#include "../control_unit/SearchInstancesByUS.h"

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



    bool setop=api_data->hasKey("reset");

    if((err = us_da->checkPresence(us_uid, presence))) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -6, "Error fetch the presence for the uid:%1%", % us_uid);
    }
    
    //UnitServer is not there in a get operation
    if(!presence && !setop) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -7, "The unit server '%1%' has not been found", % us_uid);
    }
    if(setop){
    	if(presence){
    		if(api_data->getBoolValue("reset")){
    		    		 if((err = us_da->deleteUS(us_uid))){
    		    		            LOG_AND_TROW(US_ACT_ERR, -6, boost::str(boost::format("Error removing unit server of id:%1%") % us_uid));
    		    		 }
    		}
    	} else {
    		// create the entry
    		NewUS creat;
    		creat.execute(api_data,detach_data);
    	}

    	// look for UnitServer full description
    	if(api_data->hasKey("us_desc")&& api_data->isCDataWrapperValue("us_desc")){
    		std::auto_ptr<CDataWrapper> udesc= std::auto_ptr<CDataWrapper>(api_data->getCSDataValue("us_desc"));
    		if(udesc->hasKey("cu_desc")&& udesc->isVector("cu_desc")){
    			CMultiTypeDataArrayWrapper* cu_l=udesc->getVectorValue("cu_desc");
    		             for(int cui=0;(cu_l !=NULL) && (cui<cu_l->size());cui++){
    		                 std::auto_ptr<CDataWrapper> cuw(cu_l->getCDataWrapperElementAtIndex(cui));
    		                 if(cuw->hasKey("cu_id") && cuw->hasKey("cu_type")){
    		                	 std::string cu_id= cuw->getStringValue("cu_id");
    		                	 std::string cu_type= cuw->getStringValue("cu_type");
    		                	 US_ACT_DBG<< "adding cu :"<<cu_id<<"("<<cu_type<<") to "<<us_uid;
    		                	 if((err = us_da->addCUType(us_uid, cu_type))) {
    		                	         LOG_AND_TROW_FORMATTED(US_ACT_ERR, err, "Error adding cu '%1%' type: %2% to unit server: %3%", %cu_id%us_uid%cu_type);
    		                	  }
    		                	 chaos::metadata_service::api::control_unit::SetInstanceDescription is;
    		                	 is.execute(cuw.get(),detach_data);
    		                	 US_ACT_DBG<< "added "<<cuw->getJSONString();

    		                 }

    		             }

    	}
    	}
    } else {
    	CDataWrapper tot_res;
        chaos::common::data::CDataWrapper *result = NULL;
        CDataWrapper *res;
    	// read US
        tot_res.addStringValue("ndk_uid",us_uid);
        if((err = us_da->getDescription(us_uid, &res))) {
                 LOG_AND_TROW(US_ACT_ERR, err, "Error fetching unit server decription")
        }
        if(res->hasKey(NodeDefinitionKey::NODE_DESC)){
        	tot_res.addStringValue(NodeDefinitionKey::NODE_DESC,res->getStringValue(NodeDefinitionKey::NODE_DESC));
        }
        std::vector<boost::shared_ptr<CDataWrapper> > page_result;
        uint32_t last_sequence_id = 0;
        uint32_t page_length = 1000;
        std::vector<std::string> cu_type_filter;

    	  GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -2)
    	    if((err = cu_da->searchInstanceForUnitServer(page_result,
    	    		us_uid,
    	                                                 cu_type_filter,
    	                                                 last_sequence_id,
    	                                                 page_length))) {
    	    	  LOG_AND_TROW_FORMATTED(US_ACT_ERR, err, "Error performing search page on %1%",%us_uid)
    	    } else {
    	        result = new CDataWrapper();

    	        if(page_result.size() > 0) {
    	            //add found element to result
    	            CDataWrapper* res;


    	            for (std::vector<boost::shared_ptr<CDataWrapper> >::iterator it = page_result.begin();
    	                 it != page_result.end();
    	                 it++) {
    	            	if(it->get()->hasKey("ndk_uid")){
        	            	std::string cu_uid;

        	            	cu_uid = it->get()->getStringValue("ndk_uid");
    	            	 if((err = cu_da->getInstanceDescription(cu_uid,
    	            	                                            &res))){
    	            	        LOG_AND_TROW(US_ACT_ERR, err, boost::str(boost::format("Error fetching the control unit instance description for cuid:%1%") % cu_uid));

    	            	 }
	            	    tot_res.appendCDataWrapperToArray(*res);

    	            }
    	        }
    	       tot_res.finalizeArrayForKey("cu_desc");
    	    }
     	       result->addCSDataValue("us_desc",tot_res);

    	    return result;


    }
    }

    return NULL;
}
