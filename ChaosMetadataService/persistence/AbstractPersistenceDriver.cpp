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
#include "AbstractPersistenceDriver.h"

using namespace chaos::metadata_service::persistence;

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
    CHK_AND_DELETE_OBJ_POINTER(node_da_instance)
    CHK_AND_DELETE_OBJ_POINTER(unit_server_da_instance)
}

// return the implementation of the producer data access
data_access::NodeDataAccess *AbstractPersistenceDriver::getNodeDataAccess() {
    CHAOS_ASSERT(node_da_instance)
    return (data_access::NodeDataAccess *)node_da_instance;
}

// return the implementation of the unit server data access
data_access::UnitServerDataAccess *AbstractPersistenceDriver::getUnitServerDataAccess() {
    CHAOS_ASSERT(unit_server_da_instance);
    return (data_access::UnitServerDataAccess *)unit_server_da_instance;
}

// return the implementation of the producer data access
data_access::ControlUnitDataAccess *AbstractPersistenceDriver::getControlUnitDataAccess() {
    CHAOS_ASSERT(control_unit_da_instance)
    return (data_access::ControlUnitDataAccess *)control_unit_da_instance;
}

// return the implementation of the unit server data access
data_access::UtilityDataAccess *AbstractPersistenceDriver::getUtilityDataAccess() {
    CHAOS_ASSERT(utility_da_instance)
    return (data_access::UtilityDataAccess *)utility_da_instance;
}