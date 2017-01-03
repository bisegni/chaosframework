/*
 *	PersistenceManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 14/12/2016 INFN, National Institute of Nuclear Physics
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

#include "PersistenceManager.h"
#include "../ChaosMetadataService.h"

using namespace chaos::common::utility;
using namespace chaos::metadata_service;
using namespace chaos::metadata_service::persistence;

PersistenceManager::PersistenceManager() {
    
}

PersistenceManager::~PersistenceManager() {
    
}

void PersistenceManager::init(void* init_data) throw (chaos::CException) {
    const std::string impl_name = ChaosMetadataService::getInstance()->setting.persistence_implementation+"PersistenceDriver";
    persistence_driver.reset(ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(impl_name),
                             impl_name);
    if(persistence_driver.get() == NULL) throw chaos::CException(-1, "No Persistence driver found", __PRETTY_FUNCTION__);
    persistence_driver.init(NULL, __PRETTY_FUNCTION__);
}

void PersistenceManager::deinit() throw (chaos::CException) {
    persistence_driver.deinit(NULL);
}
