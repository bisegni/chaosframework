/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
using namespace chaos::common::data::structured;

using namespace chaos::service_common::data::script;

using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

ExecutionPoolHeartbeat::ExecutionPoolHeartbeat():
AbstractApi("executionPoolHeartbeat"){}

ExecutionPoolHeartbeat::~ExecutionPoolHeartbeat() {}

chaos::common::data::CDataWrapper *ExecutionPoolHeartbeat::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_PARENT, ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%chaos::NodeDefinitionKey::NODE_PARENT));
    
    //fetch the unit server that has generated the hertbeat operation
    const std::string us_uid = CDW_GET_VALUE_WITH_DEFAULT(api_data,
                                                          chaos::NodeDefinitionKey::NODE_PARENT,
                                                          getStringValue,
                                                          "");
    DBG << "Heart beat received by " << us_uid;
    
    ChaosStringVector epool_list;
    ChaosStringVector running_script_instance_list;
    
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -3)
    
    /*if(api_data->hasKey(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LIST)) {
        CHAOS_LASSERT_EXCEPTION(api_data->isVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LIST), ERR, -4, "The running instance list key need to be a vector");
        
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> array(api_data->getVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LIST));
        for(int idx = 0;
            idx < array->size();
            idx++) {
            running_script_instance_list.push_back(array->getStringElementAtIndex(idx));
        }
        
        //fire the heartbeat on the instance list
        if((err = s_da->instanceForUnitServerHeartbeat(running_script_instance_list,
                                                       us_uid,
                                                       30000))){
            LOG_AND_TROW_FORMATTED(ERR, err, "Error during heartbeating peration for running script on unit server %1%", %us_uid);
        }
    }*/
    
    if(api_data->hasKey(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST)) {
        CHAOS_LASSERT_EXCEPTION(api_data->isVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST), ERR, -6, "The execution pool list key need to be a vector");
        std::vector<chaos::service_common::data::script::ScriptBaseDescription> current_script_page;
        
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> array(api_data->getVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST));
        for(int idx = 0;
            idx < array->size();
            idx++) {
            epool_list.push_back(array->getStringElementAtIndex(idx));
        }
        
        //check if we have script to load
        if(s_da->getScriptForExecutionPoolPathList(epool_list,
                                                   current_script_page,
                                                   0,
                                                   1)){
            LOG_AND_TROW(ERR, -4, "Error seraching script for epoolist");
        }
        //check if we have almost one script to execute.
        if(current_script_page.size() > 0) {
            
            //all is gone weel, we have script to load
            ChaosUniquePtr<chaos::common::data::CDataWrapper> batch_data(new CDataWrapper());
            //copy to batch data unit server name and pool list
            batch_data->addStringValue(chaos::NodeDefinitionKey::NODE_PARENT, us_uid);
            api_data->copyKeyTo(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST, *batch_data);
            
            //load operation is done in the batch system
            getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::script::LoadInstanceOnUnitServer),
                                              batch_data.release(),
                                              0,
                                              1000);
        }
    }
    
    return NULL;
}
