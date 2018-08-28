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

#include <chaos_metadata_service_client/api_proxy/data_service/UpdateDS.h>

using namespace chaos;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::data_service;

API_PROXY_CD_DEFINITION(UpdateDS,
                        "data_service",
                        "update")

/*!

 */
ApiProxyResult UpdateDS::execute(const std::string& ds_unique_id,
                                 const std::string& direct_io_address,
                                 int32_t direct_io_endpoint) {

    common::data::CDataWrapper *message = new common::data::CDataWrapper();
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, ds_unique_id);
    message->addStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR, direct_io_address);
    message->addInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT, direct_io_endpoint);
    return callApi(message);
}
