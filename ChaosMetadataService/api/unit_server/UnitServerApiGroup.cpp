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

#include "UnitServerApiGroup.h"
#include "GetDescription.h"
#include "LoadUnloadControlUnit.h"
#include "NewUS.h"
#include "ManageCUType.h"
#include "DeleteUS.h"
#include "GetSetFullUnitServer.h"

using namespace chaos::metadata_service::api::unit_server;
DEFINE_CLASS_FACTORY_NO_ALIAS(UnitServerApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

UnitServerApiGroup::UnitServerApiGroup():
AbstractApiGroup(UnitServerNodeDomainAndActionRPC::RPC_DOMAIN){
	//add api for producer registration
	addApi<GetDescription>();
    addApi<LoadUnloadControlUnit>();
    addApi<NewUS>();
    addApi<DeleteUS>();
    addApi<ManageCUType>();
    addApi<GetSetFullUnitServer>();
}

UnitServerApiGroup::~UnitServerApiGroup() {
	
}
