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
//----------------------------servcie----------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/service/ResetAll.h>

//----------------------------data service nodes---------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/data_service/NewDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/UpdateDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/DeleteDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/GetAllDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/GetBestEndpoints.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/GetAssociationByDS.h>
#include <ChaosMetadataServiceClient/api_proxy/data_service/AssociationControlUnit.h>

    //----------------------------control unit nodes---------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/control_unit/StartStop.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/InitDeinit.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/GetInstance.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/CopyInstance.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/RecoverError.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/DeleteInstance.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/GetCurrentDataset.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SearchInstancesByUS.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInstanceDescription.h>
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInputDatasetAttributeValues.h>

    //----------------------------unit server nodes----------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/unit_server/NewUS.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/ManageCUType.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/GetDescription.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/LoadUnloadControlUnit.h>

//------------------------------abstract nodes-----------------------------------------
#include <ChaosMetadataServiceClient/api_proxy/node/NodeSearch.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandGet.h>
#include <ChaosMetadataServiceClient/api_proxy/node/UpdateProperty.h>
#include <ChaosMetadataServiceClient/api_proxy/node/ChangeNodeState.h>
#include <ChaosMetadataServiceClient/api_proxy/node/GetNodeDescription.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSet.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateGet.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateDelete.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSearch.h>
#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSubmit.h>
#include <ChaosMetadataServiceClient/api_proxy/node/GetCommandAndTemplateDescription.h>

#endif
