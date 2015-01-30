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

AbstractPersistenceDriver::AbstractPersistenceDriver(const std::string& name):
NamedService(name){
	
}

AbstractPersistenceDriver::~AbstractPersistenceDriver() {
	
}

//! return the implementation of the producer data access
data_access::ProducerDataAccess *AbstractPersistenceDriver::getProducerDataAccess() {
    CHAOS_ASSERT(map_dataaccess_instances[data_access_type::DataAccessTypeProducer])
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
    return static_cast<data_access::ProducerDataAccess*>(map_dataaccess_instances[data_access_type::DataAccessTypeProducer]);
}

//! return the implementation of the unit server data access
data_access::UnitServerDataAccess *AbstractPersistenceDriver::getUnitServerDataAccess() {
    CHAOS_ASSERT(map_dataaccess_instances[data_access_type::DataAccessTypeProducer])
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
    return static_cast<data_access::UnitServerDataAccess*>(map_dataaccess_instances[data_access_type::DataAccessTypeUnitServer]);
}