/*
 *	CopyInstance.cpp
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

#include "CopyInstance.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_CI_INFO INFO_LOG(CopyInstance)
#define CU_CI_DBG  DBG_LOG(CopyInstance)
#define CU_CI_ERR  ERR_LOG(CopyInstance)

CopyInstance::CopyInstance():
AbstractApi("copyInstance"){
    
}

CopyInstance::~CopyInstance() {
    
}

CDataWrapper *CopyInstance::execute(CDataWrapper *api_data,
                                    bool& detach_data) throw(chaos::CException) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, CU_CI_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "ndk_uid_cu_src", CU_CI_ERR, -2, "The ndk_uid_cu_src key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "ndk_uid_us_src", CU_CI_ERR, -3, "The ndk_uid_us_src key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "ndk_uid_cu_dst", CU_CI_ERR, -4, "The ndk_uid_cu_dst key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "ndk_uid_us_dst", CU_CI_ERR, -5, "The ndk_uid_us_dst key is mandatory")
    
    //get node data access
    int err = 0;
    bool presence = false;
    std::vector<std::string> keys;
    CDataWrapper *tmp_ptr = NULL;
    
    std::auto_ptr<CDataWrapper> source_instance;
    std::auto_ptr<CDataWrapper> destination_instance(new CDataWrapper());
    
    const std::string cu_src = api_data->getStringValue("ndk_uid_cu_src");
    const std::string us_src = api_data->getStringValue("ndk_uid_us_src");
    const std::string cu_dst = api_data->getStringValue("ndk_uid_cu_dst");
    const std::string us_dst = api_data->getStringValue("ndk_uid_us_dst");
    
    bool different_us = (us_src.compare(us_dst) != 0);
    //!
    bool same_cui_id = (cu_src.compare(cu_dst) == 0);
    bool move_operation = api_data->hasKey("move")?api_data->getBoolValue("move"):false;
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -6)
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -6)
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -6)
    if((err = cu_da->getInstanceDescription(us_src, cu_src, &tmp_ptr))){
        LOG_AND_TROW_FORMATTED(CU_CI_ERR, -7, "Error (%3%) getting instance for control unit %1% and unit server %2%", %cu_src%us_src%err)
    } else if(tmp_ptr == NULL) {
        LOG_AND_TROW_FORMATTED(CU_CI_ERR, -8, "No instance found for control unit %1% and unit server %2%", %cu_src%us_src)
        
    }
    source_instance.reset(tmp_ptr);
    
    CHECK_KEY_THROW_AND_LOG(source_instance.get(),
                            "control_unit_implementation",
                            CU_CI_ERR,
                            -9,
                            "The source control unit istance doesnt have the control unit type");
    
    if(different_us) {
        //move to another unit server so we need to add the control unit type to this us
        
        //add the new control unit
        us_da->addCUType(us_dst, source_instance->getStringValue("control_unit_implementation"));
        
        if(same_cui_id) {
            //control unit need to be move so we need to erase the instance for it befor add it
            if((err = cu_da->deleteInstanceDescription(us_src, cu_src))){
                LOG_AND_TROW_FORMATTED(CU_CI_ERR, -7, "Error (%3%) erasing instance for control unit %1% and unit server %2%", %cu_src%us_src%err)
            }
        }
        
    }
    //copy within the same unit server
    //compose destination instane changing only the unit server that host the instance
    source_instance->getAllKey(keys);
    for(std::vector<std::string>::iterator it = keys.begin();
        it != keys.end();
        it++) {
        if(it->compare(NodeDefinitionKey::NODE_PARENT) == 0){
            destination_instance->addStringValue(NodeDefinitionKey::NODE_PARENT, us_dst);
        } else {
            source_instance->copyKeyTo(*it, *destination_instance.get());
        }
    }
    
    //check if control unit node exists
    if((err = cu_da->checkPresence(cu_dst,
                                   presence))){
        LOG_AND_TROW_FORMATTED(CU_CI_ERR, -9, "Error (%1%) checking presence for control unit %2%", %err%cu_dst)
    }
    if (!presence) {
        //add new control unit node
        std::auto_ptr<CDataWrapper> node_min_dec(new CDataWrapper());
        node_min_dec->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_dst);
        node_min_dec->addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
        //need to be create a new empty node
        if((err = cu_da->insertNewControlUnit(*node_min_dec.get()))) {
            LOG_AND_TROW_FORMATTED(CU_CI_ERR, -10, "Error (%1%) creating node for control unit %2%", %err%cu_dst)
        }
    }
    if((err = cu_da->setInstanceDescription(cu_dst, *destination_instance.get()))) {
        LOG_AND_TROW_FORMATTED(CU_CI_ERR, -11, "Error (%1%) setting the instance for control unit %2%", %err%cu_dst)
    }
    
    if(move_operation &&
       same_cui_id == false) {
        //we need to remove the
        n_da->deleteNode(cu_src);
    }
    return NULL;
}
