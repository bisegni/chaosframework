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

#include <chaos_metadata_service_client/api_proxy/unit_server/NewUS.h>

using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::unit_server;

API_PROXY_CD_DEFINITION(NewUS,
                        "unit_server",
                        "newUS")

/*!

 */
ApiProxyResult NewUS::execute(const std::string& new_unit_server_uid,const std::string desc,const chaos::common::data::CDataWrapper*custom) {
    chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, new_unit_server_uid);
    message->addStringValue(chaos::NodeDefinitionKey::NODE_DESC, desc);

    return callApi(message);
}
