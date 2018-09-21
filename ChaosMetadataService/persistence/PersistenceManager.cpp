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

#include "PersistenceManager.h"
#include "../ChaosMetadataService.h"

using namespace chaos::common::utility;
using namespace chaos::metadata_service;
using namespace chaos::metadata_service::persistence;

PersistenceManager::PersistenceManager() {
    
}

PersistenceManager::~PersistenceManager() {
    
}

void PersistenceManager::init(void* init_data)  {
    const std::string impl_name = ChaosMetadataService::getInstance()->setting.persistence_implementation+"PersistenceDriver";
    persistence_driver.reset(ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(impl_name),
                             impl_name);
    if(persistence_driver.get() == NULL) throw chaos::CException(-1, "No Persistence driver found", __PRETTY_FUNCTION__);
    persistence_driver.init(NULL, __PRETTY_FUNCTION__);
}

void PersistenceManager::deinit()  {
    persistence_driver.deinit(__PRETTY_FUNCTION__);
}
