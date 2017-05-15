/*
 *	CopyInstance.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/control_unit/CopyInstance.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;


API_PROXY_CD_DEFINITION(CopyInstance,
                        "control_unit",
                        "copyInstance")

/*!
 
 */
ApiProxyResult CopyInstance::execute(const std::string& cu_src,
                                     const std::string& us_src,
                                     const std::string& cu_dst,
                                     const std::string& us_dst) {
    std::auto_ptr<CDataWrapper> message(new chaos::common::data::CDataWrapper());

    //compose data pack
    message->addStringValue("ndk_uid_cu_src", cu_src);
    message->addStringValue("ndk_uid_us_src", us_src);
    message->addStringValue("ndk_uid_cu_dst", cu_dst);
    message->addStringValue("ndk_uid_us_dst", us_dst);
    
    //call api
    return callApi(message.release());
}
