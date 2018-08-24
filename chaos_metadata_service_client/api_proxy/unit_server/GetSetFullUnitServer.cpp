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

#include <chaos_metadata_service_client/api_proxy/unit_server/GetSetFullUnitServer.h>
#include "NewUS.h"
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::unit_server;

API_PROXY_CD_DEFINITION(GetSetFullUnitServer,
                        "unit_server",
                        "GetSetFullUnitServer");

ApiProxyResult GetSetFullUnitServer::execute(const std::string& unit_server_uid, int reset, const chaos::common::data::CDataWrapper* usdesc) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, unit_server_uid);
    if(usdesc){
    	message->addCSDataValue("us_desc",*usdesc);
    }
    message->addInt32Value("reset", reset);
    return callApi(message);
}

ApiProxyResult GetSetFullUnitServer::execute(const std::string& unit_server_uid) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, unit_server_uid);
    return callApi(message);
}
