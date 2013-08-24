/*
 *	DriverManager.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <boost/functional/hash.hpp>

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/driver_manager/DriverManager.h>


namespace common_utility = chaos::common::utility;
namespace cu_driver_manager = chaos::cu::driver_manager;
namespace cu_driver = chaos::cu::driver_manager::driver;
namespace common_plugin = chaos::common::plugin;

#define DMLAPP_ LAPP_ << "[DriverManager] "
#define DMLDBG_ LDBG_ << "[DriverManager] "
#define DMLERR_ LERR_ << "[DriverManager] "

/*
 Constructor
 */
cu_driver_manager::DriverManager::DriverManager() {
	
}

/*
 Desctructor
 */
cu_driver_manager::DriverManager::~DriverManager() {
	
}

// Initialize instance
void cu_driver_manager::DriverManager::init(void *initParameter) throw(chaos::CException) {
	
}

// Start the implementation
void cu_driver_manager::DriverManager::start() throw(chaos::CException) {
	
}

// Stop the implementation
void cu_driver_manager::DriverManager::stop() throw(chaos::CException) {
	
}


// Deinit the implementation
void cu_driver_manager::DriverManager::deinit() throw(chaos::CException) {
	boost::mutex::scoped_lock lock(mutextMapAccess);

	//deinitialize all allcoated driver
	mapParameterHashLiveInstance.clear();
	for(std::map<string, cu_driver::AbstractDriver*>::iterator it = mapDriverUUIDHashLiveInstance.begin();
		it != mapDriverUUIDHashLiveInstance.end();
		it++){
		
		//deinitialize driver
		try {
			DMLAPP_ << "Deinitializing device driver with uuid = " << it->second->driverUUID;
			chaos::utility::InizializableService::deinitImplementation(it->second, "AbstractDriver", "DriverManager::deinit");
		} catch (...) {
			DMLAPP_ << "Error deinitializing device driver with uuid = " << it->second->driverUUID;
			
		}
		DMLAPP_ << "Deleting device driver with uuid = " << it->second->driverUUID;
		delete(it->second);
	}
	mapDriverUUIDHashLiveInstance.clear();
}

// Register a new driver
void cu_driver_manager::DriverManager::registerDriver(boost::shared_ptr< common_utility::ObjectInstancer<cu_driver::AbstractDriver> > instancer,
													  boost::shared_ptr< common_plugin::PluginInspector > description) throw(chaos::CException) {
	
	if(!instancer)
		throw chaos::CException(1, "The instancer of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	if(!description)
		throw chaos::CException(2, "The description of the driver is mandatory for his registration", "DriverManager::registerDriver");

	std::string composedDriverName = description->getName();
	composedDriverName.append(description->getVersion());
	DMLAPP_ << "Register new driver with alias = " << composedDriverName;

	if(mapDriverAliasVersionInstancer.count(composedDriverName)) {
		DMLAPP_ << "Driver is already registered";
		return;
	}
	
	mapDriverAliasDescription.insert(make_pair(composedDriverName, description));
	
	mapDriverAliasVersionInstancer.insert(make_pair(composedDriverName, instancer));
}

// Get a new driver accessor for a driver instance
cu_driver::DriverAccessor *cu_driver_manager::DriverManager::getNewAccessorForDriverInstance(cu_driver::DrvRequestInfo& request_info) throw(chaos::CException) {
    boost::mutex::scoped_lock lock(mutextMapAccess);
	
	size_t instanceHash;
	std::string composedDriverName;
	std::string inputParameterHashing;
	cu_driver::DriverAccessor *accessor = NULL;

	boost::hash<std::string> string_hash;
	// the hashing of th einstance  is composed by name+version+input_parameter[if given], because
	// different device drive can have same parameter names. If no input is passed the hashing is
	// calculated using only the device name and the version.
	
	//befor hashing we need to check if the driver with alias and version has been registered
	DMLDBG_ << "Has been requested a driver with the information -> " << request_info.alias << "/" << request_info.version << "/" << request_info.init_parameter;
	composedDriverName = request_info.alias;
	composedDriverName.append(request_info.version);
	
	inputParameterHashing = composedDriverName;
	inputParameterHashing.append(request_info.init_parameter);
	
	instanceHash = string_hash(inputParameterHashing);
	DMLDBG_ << "The hash value for the driver is -> " << instanceHash;

	if(mapParameterHashLiveInstance.count(instanceHash)) {
		DMLDBG_ << "Driver is already been instantiated";

		if(mapParameterHashLiveInstance[instanceHash]->getNewAccessor(&accessor)) {
			//new accessor has been allocated
			DMLAPP_ << "Got new driver accessor with index ="<< accessor->accessor_index << " from driver with uuid =" << mapParameterHashLiveInstance[instanceHash];

			return accessor;
		} else {
			throw chaos::CException(1, "Device driver get new accessor", "DriverManager::getNewAccessorForDriverInstance");
		}
	}
	DMLDBG_ << "Driver need to be instantiated using alias =" << composedDriverName << " instancer presence = " << mapDriverAliasVersionInstancer.count(composedDriverName);
	//the instance of the driver need to be created
	if(!mapDriverAliasVersionInstancer.count(composedDriverName))
	   throw chaos::CException(1, "The isntance of the drive has not been found", "DriverManager::getNewAccessorForDriverInstance");
	
	// i can create the instance
	cu_driver::AbstractDriver *driverInstance = mapDriverAliasVersionInstancer[composedDriverName]->getInstance();
	driverInstance->identificationHash = instanceHash;
	//initialize the newly create instance
	DMLAPP_ << "Initilizing device driver with uuid = " << driverInstance->driverUUID;
	chaos::utility::InizializableService::initImplementation(driverInstance, (void*)(request_info.init_parameter), "AbstractDriver", "DriverManager::getNewAccessorForDriverInstance");
	
	//here the driver has been initializated and has been associated with the hash of the parameter
	DMLAPP_ << "Add device driver with hash = " << instanceHash;
	mapParameterHashLiveInstance.insert(make_pair(instanceHash, driverInstance));
	mapDriverUUIDHashLiveInstance.insert(make_pair(driverInstance->driverUUID, driverInstance));
	//now can get new accessor
	if(driverInstance->getNewAccessor(&accessor)) {
		DMLAPP_ << "Got new driver accessor with index ="<< accessor->accessor_index << " from driver with uuid =" << driverInstance->driverUUID;
	}
	return accessor;
}

//! release the accessor instance
void cu_driver_manager::DriverManager::releaseAccessor(cu_driver::DriverAccessor *accessor) {
	boost::mutex::scoped_lock lock(mutextMapAccess);

	if(!mapDriverUUIDHashLiveInstance.count(accessor->driver_uuid)) return;
	
	cu_driver::AbstractDriver *dInstance = mapDriverUUIDHashLiveInstance[accessor->driver_uuid];
	//we can relase the accessor
	if(!dInstance->releaseAccessor(accessor)) {
		throw chaos::CException(1, "This errore never have to appen", "DriverManager::releaseAccessor");
	}
	if(!dInstance->accessors.size()) {
		DMLAPP_ << "The driver with uuid =" << dInstance->driverUUID << " has no more accessor allocated so we can deinitialize it";
		//deinitialize driver
		try {
			DMLAPP_ << "Deinitializing device driver with uuid = " << dInstance->driverUUID;
			chaos::utility::InizializableService::deinitImplementation(dInstance, "AbstractDriver", "DriverManager::deinit");
		} catch (...) {
			DMLAPP_ << "Error deinitializing device driver with uuid = " << dInstance->driverUUID;
			
		}
		DMLAPP_ << "Deleting device driver with uuid = " << dInstance->driverUUID;
		mapParameterHashLiveInstance.erase(dInstance->identificationHash);
		mapDriverUUIDHashLiveInstance.erase(dInstance->driverUUID);
		delete(dInstance);
		dInstance = NULL;
	}
}