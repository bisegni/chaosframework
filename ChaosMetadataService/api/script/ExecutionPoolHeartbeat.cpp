/*
 *	ExecutionPoolHeartbeat.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/06/16 INFN, National Institute of Nuclear Physics
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

#include "ExecutionPoolHeartbeat.h"
#include "../../batch/script/LoadInstanceOnUnitServer.h"

#include <chaos/common/utility/TimingUtil.h>

#include <chaos_service_common/data/data.h>

#define INFO INFO_LOG(ExecutionPoolHeartbeat)
#define DBG  DBG_LOG(ExecutionPoolHeartbeat)
#define ERR  ERR_LOG(ExecutionPoolHeartbeat)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::data::general;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

ExecutionPoolHeartbeat::ExecutionPoolHeartbeat():
AbstractApi("executionPoolHeartbeat"){}

ExecutionPoolHeartbeat::~ExecutionPoolHeartbeat() {}

chaos::common::data::CDataWrapper *ExecutionPoolHeartbeat::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_PARENT, ERR, -2, "The script seq is mandatory");
    
    //fetch the unit server that has generated the hertbeat operation
    const std::string us_uid = CDW_GET_VALUE_WITH_DEFAULT(api_data,
                                                          chaos::NodeDefinitionKey::NODE_PARENT,
                                                          getStringValue,
                                                          "");
    DBG << "Heart beat received by " << us_uid;
    
    ChaosStringVector epool_list;
    ChaosStringVector running_script_instance_list;
    
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -3)
    
    if(api_data->hasKey("running_instance_list")) {
        CHAOS_LASSERT_EXCEPTION(api_data->isVectorValue("running_instance_list"), ERR, -4, "The running instance list key need to be a vector");
        
        std::auto_ptr<CMultiTypeDataArrayWrapper> array(api_data->getVectorValue("running_instance_list"));
        for(int idx = 0;
            idx < array->size();
            idx++) {
            running_script_instance_list.push_back(array->getStringElementAtIndex(idx));
        }
        
        //fire the heartbeat on the instance list
        if((err = s_da->instanceForUnitServerHeartbeat(running_script_instance_list,
                                                       us_uid,
                                                       TimingUtil::getTimeStamp()))){
            LOG_AND_TROW_FORMATTED(ERR, err, "Error during heartbeating peration for running script on unit server %1%", %us_uid);
        }
    }
    
    if(api_data->hasKey("ep_pool_list")) {
        CHAOS_LASSERT_EXCEPTION(api_data->isVectorValue("ep_pool_list"), ERR, -6, "The execution pool list key need to be a vector");
                //all is gone weel
        std::auto_ptr<CDataWrapper> batch_data(new CDataWrapper());
        //copy to batch data unit server name and pool list
        batch_data->addStringValue(chaos::NodeDefinitionKey::NODE_PARENT, us_uid);
        batch_data->copyKeyTo("ep_pool_list", *batch_data);
        
        //load operation is done in the batch system
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::script::LoadInstanceOnUnitServer),
                                          batch_data.release(),
                                          0,
                                          1000);
    }
    
    return NULL;
}