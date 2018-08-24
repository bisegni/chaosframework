/*
 * Copyright 2012, 03/07/2018 INFN
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

#include <chaos_metadata_service_client/api_proxy/control_unit/SendStorageBurst.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;


API_PROXY_CD_DEFINITION(SendStorageBurst,
                        "control_unit",
                        "sendStorageBurst")

ApiProxyResult SendStorageBurst::execute(const std::string& cu_uid,
                                         chaos::common::data::structured::DatasetBurst& dataset_burst) {
    ChaosStringSet cu_set;
    cu_set.insert(cu_uid);
    return execute(cu_set, dataset_burst);
}

ApiProxyResult SendStorageBurst::execute(const ChaosStringSet& cu_set,
                                         chaos::common::data::structured::DatasetBurst& dataset_burst) {
    DatasetBurstSDWrapper db_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(DatasetBurst, dataset_burst));
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new chaos::common::data::CDataWrapper());
    for(ChaosStringSetIterator it = cu_set.begin(),
        end = cu_set.end();
        it != end;
        it++) {
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    db_sdw.serialize()->copyAllTo(*message);
    return callApi(message);
}
