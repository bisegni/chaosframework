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

#include <chaos_metadata_service_client/api_proxy/node/CommandTemplateSearch.h>

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
