/*
 *	SearchScript.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#include "SearchScript.h"

#define INFO INFO_LOG(SearchScript)
#define DBG  DBG_LOG(SearchScript)
#define ERR  ERR_LOG(SearchScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::api::script;

SearchScript::SearchScript():
AbstractApi("getLogForNodeUID"){
}

SearchScript::~SearchScript() {
}

chaos::common::data::CDataWrapper *SearchScript::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    std::auto_ptr<CDataWrapper> result;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    
  
    return result.release();
}