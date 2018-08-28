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

#include <chaos_metadata_service_client/api_proxy/service/CreateNewSnapshot.h>

using namespace chaos;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(CreateNewSnapshot,
                        "service",
                        "createNewSnapshot")

/*!
 
 */
ApiProxyResult CreateNewSnapshot::execute(const std::string& snapshot_name,
                                          const SnapshotNodeList& node_list) {
    common::data::CDataWrapper *message = new common::data::CDataWrapper();
    message->addStringValue("snapshot_name", snapshot_name);
    
    for(SnapshotNodeListConstIterator it = node_list.begin();
        it != node_list.end();
        it++) {
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey("node_list");
    return callApi(message);
}
