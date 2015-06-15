//
//  CommandInstanceSubmit.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 15/06/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "CommandInstanceSubmit.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_CIS_INFO INFO_LOG(CommandInstanceSubmit)
#define N_CIS_DBG  DBG_LOG(CommandInstanceSubmit)
#define N_CIS_ERR  ERR_LOG(CommandInstanceSubmit)

CommandInstanceSubmit::CommandInstanceSubmit():
AbstractApi("commandInstanceSubmit"){
    
}

CommandInstanceSubmit::~CommandInstanceSubmit() {
    
}

CDataWrapper *CommandInstanceSubmit::execute(CDataWrapper *api_data,
                                             bool& detach_data) throw(chaos::CException) {
    int err = 0;
    bool presence = false;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, N_CIS_ERR, -2, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_name", N_CIS_ERR, -3, "The attribute template_name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_CIS_ERR, -4, "The attribute for command unique id is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    return result;
}