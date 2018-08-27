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

#include <chaos_metadata_service_client/api_proxy/control_unit/CopyInstance.h>

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
    CDWUniquePtr message(new chaos::common::data::CDataWrapper());

    //compose data pack
    message->addStringValue("ndk_uid_cu_src", cu_src);
    message->addStringValue("ndk_uid_us_src", us_src);
    message->addStringValue("ndk_uid_cu_dst", cu_dst);
    message->addStringValue("ndk_uid_us_dst", us_dst);
    
    //call api
    return callApi(message);
}
