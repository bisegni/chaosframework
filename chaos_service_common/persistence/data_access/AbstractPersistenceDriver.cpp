/*
 *	AbstractPersistenceDriver.cpp
 *	!CHOAS
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