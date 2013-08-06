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

#include <chaos/cu_toolkit/driver_manager/DriverManager.h>


namespace common_utility = chaos::common::utility;
namespace cu_driver_manager = chaos::cu::driver_manager;
namespace cu_driver = chaos::cu::driver_manager::driver;

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
void cu_driver_manager::DriverManager::registerDriver(string& alias,
										 common_utility::ObjectInstancer<cu_driver::AbstractDriver> *instancer,
										 boost::ptr_vector<cu_driver::DriverInitParameterDesc> *inputParameter) throw(chaos::CException) {
	
	if(mapDriverNameInstancer.count(alias)) {
		return;
	}
	
	if(!instancer)
		throw chaos::CException(1, "The instancer of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	boost::shared_ptr< boost::ptr_vector<cu_driver::DriverInitParameterDesc> > spInputParameter(inputParameter);
	boost::shared_ptr< common_utility::ObjectInstancer<cu_driver::AbstractDriver> > spInstancer(instancer);
	
	if(spInputParameter)
		mapDriverNameParamter.insert(make_pair(alias, spInputParameter));
	
	mapDriverNameInstancer.insert(make_pair(alias, spInstancer));
}

// Get a new driver accessor for a driver instance
cu_driver::DriverAccessor *cu_driver_manager::DriverManager::getNewAccessorForDriverInstance(std::string& alias,
																							 const chaos::CDataWrapper *initParameter) throw(chaos::CException) {
	cu_driver::DriverAccessor *accessor = NULL;
	// the hashing of the input paramiter  (in case of it presence) is composed by name+hash, because
	// different device drive can have same paramter names. If no input is passed the hashing is equal
	// to the device name.
	std::string inputParameterHashing = initParameter ? aliasName + initParameter->toHash():aliasName;
	
	if(mapParameterHashLiveInstance.count(inputParameterHashing)) {
		if(mapParameterHashLiveInstance[inputParameterHashing]->getNewAccessor(&accessor)) {
			//new accessor has been allocated
			return accessor;
		} else {
			throw chaos::CException(1, "Device driver get new accessor", "DriverManager::getNewAccessorForDriverInstance");
		}
	}
	
	//the instance of the driver need to be created
	if(!mapDriverNameInstancer.count(aliasName))
	   throw chaos::CException(2, "The isntance of the drive has not been found", "DriverManager::getNewAccessorForDriverInstance");
	
	// i can create the instance
	boost::shared_ptr< cu_driver::AbstractDriver > driverInstance(mapDriverNameInstancer[alias]->getInstance());
	
	//initialize the newly create instance
	chaos::utility::InizializableServices::initImplementation(driverInstance.get(), static_cast<void*>(initParameter), "AbstractDriver", "DriverManager::getNewAccessorForDriverInstance");
	
	//here the driver has been initializated and has been associated with the hash of the parameter
	mapParameterHashLiveInstance.insert(make_pair(inputParameterHashing, driverInstance));
	
	//now can get new accessor
	driverInstance->getNewAccessor(&accessor);
	return accessor;
}