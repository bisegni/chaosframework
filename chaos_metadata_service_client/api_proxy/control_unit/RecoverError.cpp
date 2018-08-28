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

#include <chaos_metadata_service_client/api_proxy/control_unit/RecoverError.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;


API_PROXY_CD_DEFINITION(RecoverError,
                        "control_unit",
                        "recoverError")

/*!
 
 */
ApiProxyResult RecoverError::execute(const std::vector<std::string>& cu_uids) {
    CDWUniquePtr message(new chaos::common::data::CDataWrapper());
    
    //compose data pack
    for(std::vector<std::string>::const_iterator it = cu_uids.begin();
        it != cu_uids.end();
        it++){
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    //call api
    return callApi(message);
}
