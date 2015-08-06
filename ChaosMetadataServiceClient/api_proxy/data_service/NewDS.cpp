/*
 *	NewDS.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/data_service/NewDS.h>
using namespace chaos;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::data_service;

API_PROXY_CD_DEFINITION(NewDS,
                        "data_service",
                        "new")

/*!

 */
ApiProxyResult NewDS::execute(const std::string& ds_unique_id,
                              const std::string& direct_io_address,
                              int32_t direct_io_endpoint) {
    common::data::CDataWrapper *message = new common::data::CDataWrapper();
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, ds_unique_id);
    message->addStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR, direct_io_address);
    message->addInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT, direct_io_endpoint);
    return callApi(message);
}