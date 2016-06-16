/*
 *	api.h
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
#ifndef CHAOSFramework_api_proxy_h
#define CHAOSFramework_api_proxy_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxyManager.h>

//---include all api header
#include <ChaosMetadataServiceClient/api_proxy/node/node.h>
#include <ChaosMetadataServiceClient/api_proxy/groups/groups.h>
#include <ChaosMetadataServiceClient/api_proxy/script/script.h>
#include <ChaosMetadataServiceClient/api_proxy/logging/logging.h>
#include <ChaosMetadataServiceClient/api_proxy/service/service.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/data_service.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/control_unit.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/unit_server.h>

//add all service common data
#include <chaos_service_common/data/data.h>

#endif
