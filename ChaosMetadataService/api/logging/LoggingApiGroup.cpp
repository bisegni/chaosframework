/*
 *	LoggingApiGroup.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 11/02/16 INFN, National Institute of Nuclear Physics
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

#include "LoggingApiGroup.h"
#include "SubmitEntry.h"
#include "GetLogForSourceUID.h"

using namespace chaos::metadata_service::api::logging;

DEFINE_CLASS_FACTORY_NO_ALIAS(LoggingApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

LoggingApiGroup::LoggingApiGroup():
AbstractApiGroup(MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN){
    addApi<SubmitEntry>();
    addApi<GetLogForSourceUID>();
}

LoggingApiGroup::~LoggingApiGroup() {}
