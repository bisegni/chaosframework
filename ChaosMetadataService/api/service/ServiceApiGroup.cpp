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

#include "ResetAll.h"
#include "GetAllSnapshot.h"
#include "DeleteSnapshot.h"
#include "RestoreSnapshot.h"
#include "ServiceApiGroup.h"
#include "CreateNewSnapshot.h"
#include "GetSnapshotForNode.h"
#include "GetNodesForSnapshot.h"
#include "GetSnapshotDatasetsForNode.h"
#include "SetSnapshotDatasetsForNode.h"

#include "SetVariable.h"
#include "GetVariable.h"
#include "RemoveVariable.h"

using namespace chaos::metadata_service::api::service;

DEFINE_CLASS_FACTORY_NO_ALIAS(ServiceApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

ServiceApiGroup::ServiceApiGroup():
AbstractApiGroup("service"){
    addApi<ResetAll>();
    addApi<GetAllSnapshot>();
    addApi<DeleteSnapshot>();
    addApi<RestoreSnapshot>();
    addApi<CreateNewSnapshot>();
    addApi<GetSnapshotForNode>();
    addApi<GetNodesForSnapshot>();
    addApi<SetSnapshotDatasetsForNode>();
    addApi<GetSnapshotDatasetsForNode>();
    
    addApi<SetVariable>();
    addApi<GetVariable>();
    addApi<RemoveVariable>();
}

ServiceApiGroup::~ServiceApiGroup() {}
