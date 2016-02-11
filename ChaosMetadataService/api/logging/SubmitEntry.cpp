/*
 *	SubmitEntry.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 11/02/16 INFN, National Institute of Nuclear Physics
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

#include "SubmitEntry.h"


using namespace chaos::metadata_service::api::logging;

#define L_SE_INFO INFO_LOG(SubmitEntry)
#define L_SE_DBG  DBG_LOG(SubmitEntry)
#define L_SE_ERR  ERR_LOG(SubmitEntry)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

SubmitEntry::SubmitEntry():
AbstractApi(MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOGGING_SUBMIT_ENTRY){
    
}

SubmitEntry::~SubmitEntry() {
    
}

chaos::common::data::CDataWrapper *SubmitEntry::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, L_SE_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP, L_SE_ERR, -2, "The log timestamp key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isInt64Value(MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP), L_SE_ERR, -3, "The log timestamp key needs to be an int64 value");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP, L_SE_ERR, -4, "The log domain key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN), L_SE_ERR, -5, "The log domain needs to be a string");

    return NULL;
}