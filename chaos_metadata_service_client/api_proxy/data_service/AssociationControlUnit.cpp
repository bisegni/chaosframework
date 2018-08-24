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

#include <chaos_metadata_service_client/api_proxy/data_service/AssociationControlUnit.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::data_service;

API_PROXY_CD_DEFINITION(AssociationControlUnit,
                        "data_service",
                        "associateControlUnit")

ApiProxyResult AssociationControlUnit::execute(const std::string& ds_unique_id,
                                               std::vector<std::string> control_unit_to_associate,
                                               bool associate) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(NodeDefinitionKey::NODE_PARENT, ds_unique_id);
    message->addBoolValue("associate", associate);
    for(std::vector<std::string>::iterator it = control_unit_to_associate.begin();
        it != control_unit_to_associate.end();
        it++) {
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey(NodeDefinitionKey::NODE_UNIQUE_ID);
    return callApi(message);
}
