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
#include "ControlUnitGroup.h"
#include "SetInstanceDescription.h"
#include "SearchInstancesByUS.h"
#include "GetInstance.h"
#include "DeleteInstance.h"
#include "InitDeinit.h"
#include "StartStop.h"
#include "GetCurrentDataset.h"
//#include "GetFullDescription.h"
#include "SetInputDatasetAttributeValues.h"
#include "CopyInstance.h"
#include "RecoverError.h"
#include "Delete.h"
#include "SendStorageBurst.h"
using namespace chaos::metadata_service::api::control_unit;
DEFINE_CLASS_FACTORY_NO_ALIAS(ControlUnitGroup, chaos::metadata_service::api::AbstractApiGroup);

ControlUnitGroup::ControlUnitGroup():
AbstractApiGroup("control_unit"){
        //add api for control unit registration
    addApi<SetInstanceDescription>();
    addApi<SearchInstancesByUS>();
    addApi<GetInstance>();
    addApi<DeleteInstance>();
    addApi<InitDeinit>();
    addApi<StartStop>();
    addApi<GetCurrentDataset>();
//    addApi<GetFullDescription>();
    addApi<SetInputDatasetAttributeValues>();
    addApi<CopyInstance>();
    addApi<RecoverError>();
    addApi<Delete>();
    addApi<SendStorageBurst>();
}

ControlUnitGroup::~ControlUnitGroup() {}
