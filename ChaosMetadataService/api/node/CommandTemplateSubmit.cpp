/*
 *	CommandTemplateSubmit.cpp
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

#include "CommandTemplateSubmit.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_CTS_INFO INFO_LOG(CommandTemplateSubmit)
#define N_CTS_DBG  DBG_LOG(CommandTemplateSubmit)
#define N_CTS_ERR  ERR_LOG(CommandTemplateSubmit)

CommandTemplateSubmit::CommandTemplateSubmit():
AbstractApi("commandTemplateSubmit"){
    
}

CommandTemplateSubmit::~CommandTemplateSubmit() {
    
}

CDataWrapper *CommandTemplateSubmit::execute(CDataWrapper *api_data,
                                             bool& detach_data) throw(chaos::CException) {
    int err = 0;
    bool presence = false;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, N_CTS_ERR, -2, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "submission_task", N_CTS_ERR, -3, "The list of submiossion task is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> submission_task_list(api_data->getVectorValue("submission_task"));
    
    
    return result;
}