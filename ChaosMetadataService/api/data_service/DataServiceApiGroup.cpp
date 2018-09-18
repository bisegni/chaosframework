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

#include "DataServiceApiGroup.h"
#include "AssociationControlUnit.h"
#include "GetAssociationByDS.h"
#include "GetAllDataService.h"
#include "GetBestEndpoints.h"
using namespace chaos::metadata_service::api::metadata_service;
DEFINE_CLASS_FACTORY_NO_ALIAS(DataServiceApiGroup, chaos::metadata_service::api::AbstractApiGroup);

DataServiceApiGroup::DataServiceApiGroup():
AbstractApiGroup(DataServiceNodeDomainAndActionRPC::RPC_DOMAIN){
        //add api for DataService api
    addApi<GetAllDataService>();
    addApi<GetAssociationByDS>();
    addApi<AssociationControlUnit>();
    addApi<GetBestEndpoints>();
}

DataServiceApiGroup::~DataServiceApiGroup() {

}
