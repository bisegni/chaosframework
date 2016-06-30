/*
 *	ScriptGroupApi.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#include "SaveScript.h"
#include "SearchScript.h"
#include "ScriptGroupApi.h"
#include "LoadFullScript.h"
#include "ManageScriptInstance.h"
#include "ExecutionPoolHeartbeat.h"
#include "SearchInstancesForScript.h"

using namespace chaos::metadata_service::api::script;

DEFINE_CLASS_FACTORY_NO_ALIAS(ScriptGroupApi,
                              chaos::metadata_service::api::AbstractApiGroup);

ScriptGroupApi::ScriptGroupApi():
AbstractApiGroup("script"){
    addApi<SaveScript>();
    addApi<SearchScript>();
    addApi<LoadFullScript>();
    addApi<ManageScriptInstance>();
    addApi<ExecutionPoolHeartbeat>();
    addApi<SearchInstancesForScript>();
}

ScriptGroupApi::~ScriptGroupApi() {}
