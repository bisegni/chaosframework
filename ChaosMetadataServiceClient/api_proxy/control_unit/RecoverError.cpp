/*
 *	RecoverError.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/control_unit/RecoverError.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;


API_PROXY_CD_DEFINITION(RecoverError,
                        "control_unit",
                        "recoverError")

/*!
 
 */
ApiProxyResult RecoverError::execute(const std::vector<std::string>& cu_uids) {
    std::auto_ptr<CDataWrapper> message(new chaos::common::data::CDataWrapper());
    
    //compose data pack
    for(std::vector<std::string>::const_iterator it = cu_uids.begin();
        it != cu_uids.end();
        it++){
        message->appendStringToArray(*it);

    }
    
    //call api
    return callApi(message.release());
}