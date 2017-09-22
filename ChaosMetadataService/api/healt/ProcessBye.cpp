//
//  ProcessBye.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 07/05/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "ProcessBye.h"

#define H_NW_INFO INFO_LOG(ProcessBye)
#define H_NW_DBG  DBG_LOG(ProcessBye)
#define H_NW_ERR  ERR_LOG(ProcessBye)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::healt;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< ChaosSharedPtr<CDataWrapper> > ResultVector;

ProcessBye::ProcessBye():
AbstractApi("processBye"){

}

ProcessBye::~ProcessBye() {

}

chaos::common::data::CDataWrapper *ProcessBye::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    return NULL;
}
