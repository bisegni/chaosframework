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

#include "SaveScript.h"
#include "SearchScript.h"
#include "RemoveScript.h"
#include "ScriptGroupApi.h"
#include "UpdateBindType.h"
#include "LoadFullScript.h"
#include "UpdateScriptOnNode.h"
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
    addApi<RemoveScript>();
    addApi<UpdateBindType>();
    addApi<LoadFullScript>();
    addApi<UpdateScriptOnNode>();
    addApi<ManageScriptInstance>();
    addApi<ExecutionPoolHeartbeat>();
    addApi<SearchInstancesForScript>();
}

ScriptGroupApi::~ScriptGroupApi() {}
