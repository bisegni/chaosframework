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

#include <ChaosMetadataServiceClient/api_proxy/api.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxyManager.h>

//---include all api header
//----------------------------data service nodes---------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/data_service/NewDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/UpdateDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/DeleteDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/GetAllDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/GetAssociationByDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/AssociationControlUnit.h>

    //----------------------------control unit nodes---------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/control_unit/GetInstance.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/DeleteInstance.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SearchInstancesByUS.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInstanceDescription.h>

    //----------------------------unit server nodes----------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/unit_server/GetDescription.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/LoadUnloadControlUnit.h>

    //------------------------------abstract nodes-----------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/node/GetNodeDescription.h>
#include <ChaosMetadataServiceClient/api_proxy/node/NodeSearch.h>

#endif
