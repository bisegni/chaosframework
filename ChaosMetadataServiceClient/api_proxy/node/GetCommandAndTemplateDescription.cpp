/*
 *	GetCommandAndTemplateDescription.cpp
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

#include "GetCommandAndTemplateDescription.h"

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;


API_PROXY_CD_DEFINITION(GetCommandAndTemplateDescription,
                        "system",
                        "getCommandAndTemplateDescription")

/*!
 
 */
ApiProxyResult GetCommandAndTemplateDescription::execute(const std::string& template_name,
                                           const std::string& command_uid) {
    std::auto_ptr<CDataWrapper> message(new CDataWrapper());
    message->addStringValue("template_name", template_name);
    message->addStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, command_uid);
    //call api
    return callApi(message.release());
}
