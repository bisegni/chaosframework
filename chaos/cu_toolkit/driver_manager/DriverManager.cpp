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
	
}

// Register a new driver
void cu_driver_manager::DriverManager::registerDriver(boost::shared_ptr< common_utility::ObjectInstancer<cu_driver::AbstractDriver> > instancer,
													  boost::shared_ptr< common_plugin::PluginInspector > description) throw(chaos::CException) {
	
	if(!instancer)
		throw chaos::CException(1, "The instancer of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	if(!description)
		throw chaos::CException(2, "The description of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	DMLAPP_ << "Start new driver registration";
	std::string composedDriverName = description->getName();
	composedDriverName.append(description->getVersion());
	
	if(mapDriverAliasInstancer.count(composedDriverName)) {
		DMLAPP_ << "Driver is already registered";
		return;
	}
	
	mapDriverAliasDescription.insert(make_pair(composedDriverName, description));
	
	mapDriverAliasInstancer.insert(make_pair(composedDriverName, instancer));
}

// Get a new driver accessor for a driver instance
cu_driver::DriverAccessor *cu_driver_manager::DriverManager::getNewAccessorForDriverInstance(std::string& alias,
																							 std::string& version,
																							 std::string& init_paramter) throw(chaos::CException) {

	size_t instanceHash;
	std::string composedDriverName;
	std::string inputParameterHashing;
	cu_driver::DriverAccessor *accessor = NULL;

	boost::hash<std::string> string_hash;
	// the hashing of th einstance  is composed by name+version+input_parameter[if given], because
	// different device drive can have same parameter names. If no input is passed the hashing is
	// calculated using only the device name and the version.
	
	//befor hashing we need to check if the driver with alias and version has been registered
	
	composedDriverName = alias;
	composedDriverName.append(version);
	
	inputParameterHashing = composedDriverName;
	inputParameterHashing.append(init_paramter);
	
	instanceHash = string_hash(inputParameterHashing);
	
	if(mapParameterHashLiveInstance.count(instanceHash)) {
		if(mapParameterHashLiveInstance[instanceHash]->getNewAccessor(&accessor)) {
			//new accessor has been allocated
			return accessor;
		} else {
			throw chaos::CException(1, "Device driver get new accessor", "DriverManager::getNewAccessorForDriverInstance");
		}
	}
	
	//the instance of the driver need to be created
	if(!mapDriverAliasInstancer.count(inputParameterHashing))
	   throw chaos::CException(2, "The isntance of the drive has not been found", "DriverManager::getNewAccessorForDriverInstance");
	
	// i can create the instance
	cu_driver::AbstractDriver *driverInstance = mapDriverAliasInstancer[composedDriverName]->getInstance();
	
	//initialize the newly create instance
	chaos::utility::InizializableService::initImplementation(driverInstance, (void*)(init_paramter.c_str()), "AbstractDriver", "DriverManager::getNewAccessorForDriverInstance");
	
	//here the driver has been initializated and has been associated with the hash of the parameter
	mapParameterHashLiveInstance.insert(make_pair(instanceHash, driverInstance));
	
	//now can get new accessor
	driverInstance->getNewAccessor(&accessor);
	return accessor;
}