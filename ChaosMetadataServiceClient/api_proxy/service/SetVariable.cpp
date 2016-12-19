/*
 *	SetVariable.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/12/2016 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/service/SetVariable.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(SetVariable,
                        "service",
                        "setVariable")
/*!
 
 */
ApiProxyResult SetVariable::execute(const std::string& variable_name,
                                    const chaos::common::data::CDataWrapper& variable_value) {
    CDataWrapper *message = new CDataWrapper();
    message->addStringValue("variable_name", snapshot_name);
    message->addCSDataValue("variable_value", variable_value);
    return callApi(message);
}

