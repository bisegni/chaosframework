/*
 *	DataServiceApiGroup.cpp
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

#include "DataServiceApiGroup.h"
#include "NewDS.h"
#include "UpdateDS.h"
#include "DeleteDS.h"
#include "AssociationControlUnit.h"
#include "GetAssociationByDS.h"
#include "GetAllDataService.h"

using namespace chaos::metadata_service::api::data_service;
DEFINE_CLASS_FACTORY_NO_ALIAS(DataServiceApiGroup, chaos::metadata_service::api::AbstractApiGroup);

DataServiceApiGroup::DataServiceApiGroup():
AbstractApiGroup("data_service"){
        //add api for DataService api
    addApi<NewDS>();
    addApi<UpdateDS>();
    addApi<DeleteDS>();
    addApi<GetAllDataService>();
    addApi<GetAssociationByDS>();
    addApi<AssociationControlUnit>();

}

DataServiceApiGroup::~DataServiceApiGroup() {

}