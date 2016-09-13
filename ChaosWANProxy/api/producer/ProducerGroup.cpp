/*
 *	ProducerGroup.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "ProducerGroup.h"
#include "ProducerRegisterDatasetApi.h"
#include "ProducerGetLastDatasetApi.h"
#include "ProducerInsertDatasetApi.h"
#include "ProducerInsertJsonApi.h"
#include "ProducerRegisterJsonApi.h"


using namespace chaos::wan_proxy::api::producer;

ProducerGroup::ProducerGroup(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApiGroup("producer",
				 _persistence_driver){
	
	//ad api to group
	addApi<ProducerRegisterDatasetApi>();
	addApi<ProducerInsertDatasetApi>();
    addApi<ProducerInsertJsonApi>();
    addApi<ProducerRegisterJsonApi>();
	addApi<ProducerGetLastDatasetApi>();

}

ProducerGroup::~ProducerGroup() {
	
}
