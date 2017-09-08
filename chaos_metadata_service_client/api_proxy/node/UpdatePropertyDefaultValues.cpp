/*
 * Copyright 2012, 06/09/2017 INFN
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
#include <chaos_metadata_service_client/api_proxy/node/UpdatePropertyDefaultValues.h>

using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

API_PROXY_CD_DEFINITION(UpdatePropertyDefaultValues,
                        "system",
                        "updatePropertyDefaultValues")

ApiProxyResult UpdatePropertyDefaultValues::execute(const std::string& node_unique_id,
                                                    const chaos::common::property::PropertyGroup& node_property_group) {
    chaos::common::property::PropertyGroupVector group_vector;
    group_vector.push_back(node_property_group);
    return execute(node_unique_id,
                   group_vector);
}

ApiProxyResult UpdatePropertyDefaultValues::execute(const std::string& node_unique_id,
                                                    const chaos::common::property::PropertyGroupVector& node_property_group_vector) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    //add node uid
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_unique_id);
    PropertyGroupVectorSDWrapper pg_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroupVector, const_cast<chaos::common::property::PropertyGroupVector&>(node_property_group_vector)));
    pg_sdw.serialization_key = "property";
    pg_sdw.serialize()->copyAllTo(*message);
    //call api
    return callApi(message.release());
}
