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
#include <chaos/common/global.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

using namespace chaos::service_common::persistence::data_access;

#define APD_INFO INFO_LOG(AbstractPersistenceDriver)
#define APD_DBG  INFO_DBG(AbstractPersistenceDriver)
#define APD_ERR  INFO_ERR(AbstractPersistenceDriver)

AbstractPersistenceDriver::AbstractPersistenceDriver(const std::string& name):
NamedService(name){
	
}

AbstractPersistenceDriver::~AbstractPersistenceDriver() {
   
}


// Initialize the driver
void AbstractPersistenceDriver::init(void *init_data) throw (chaos::CException) {
    
}

// deinitialize the driver
void AbstractPersistenceDriver::deinit() throw (chaos::CException) {
    for(MapDAIterator i = map_data_access.begin();
        i != map_data_access.end();
        i++) {
        APD_INFO << "Delete data access:" << i->first;
        deleteDataAccess((AbstractDataAccess*)i->second);
    }
}