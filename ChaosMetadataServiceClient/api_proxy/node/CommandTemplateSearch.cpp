/*
 *	CommandTemplateSearch.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSearch.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;


API_PROXY_CD_DEFINITION(CommandTemplateSearch,
                        "system",
                        "commandTemplateSearch")

/*!
 
 */
ApiProxyResult CommandTemplateSearch::execute(const CommandUIDList& uid_list) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    for(CommandUIDListConstIterator it = uid_list.begin();
        it != uid_list.end();
        it++) {
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey("cmd_uid_fetch_list");
    //call api
    return callApi(message.release());
}
