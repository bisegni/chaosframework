/*
 *	CommandGet.cpp
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

#include "CommandGet.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_CG_INFO INFO_LOG(CommandGet)
#define CU_CG_DBG  DBG_LOG(CommandGet)
#define CU_CG_ERR  ERR_LOG(CommandGet)

CommandGet::CommandGet():
AbstractApi("commandGet"){
    
}

CommandGet::~CommandGet() {
    
}

CDataWrapper *CommandGet::execute(CDataWrapper *api_data,
                                  bool& detach_data) throw(chaos::CException) {
    int err = 0;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, CU_CG_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, CU_CG_ERR, -2, "The attribute BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    const std::string command_unique_id = api_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
    //we need to add the sequence
    if((err = n_da->getCommand(command_unique_id, &result))) {
        LOG_AND_TROW_FORMATTED(CU_CG_ERR, -4, "Error getting command for uid %2%", %command_unique_id)
    }
    return result;
}