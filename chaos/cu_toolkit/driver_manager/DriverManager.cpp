/*
 *	DriverManager.cpp
 *	!CHAOS
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


using namespace chaos::common::utility;
using namespace chaos::cu::driver_manager;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::common::plugin;

#define DMLAPP_ LAPP_ << "[DriverManager] "
#define DMLDBG_ LDBG_ << "[DriverManager] "
#define DMLERR_ LERR_ << "[DriverManager] "

/*
 Constructor
 */
DriverManager::DriverManager() {
	
}

/*
 Desctructor
 */
DriverManager::~DriverManager() {
	
}

// Initialize instance
void DriverManager::init(void *initParameter) throw(chaos::CException) {
	
}

// Start the implementation
void DriverManager::start() throw(chaos::CException) {
	
}

// Stop the implementation
void DriverManager::stop() throw(chaos::CException) {
	
}


// Deinit the implementation
void DriverManager::deinit() throw(chaos::CException) {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
	
	//deinitialize all allcoated driver
	mapParameterLiveInstance.clear();
    for(std::map<std::string, AbstractDriver*>::iterator it = mapDriverUUIDHashLiveInstance.begin();
		it != mapDriverUUIDHashLiveInstance.end();
		it++){
		
		//deinitialize driver
		try {
			DMLAPP_ << "Deinitializing device driver with uuid = " << it->second->driverUUID;
			InizializableService::deinitImplementation(it->second, "AbstractDriver", "DriverManager::deinit");
		} catch (...) {
			DMLAPP_ << "Error deinitializing device driver with uuid = " << it->second->driverUUID;
			
		}
		DMLAPP_ << "Deleting device driver with uuid = " << it->second->driverUUID;
		delete(it->second);
	}
	mapDriverUUIDHashLiveInstance.clear();
	mapParameterLiveInstance.clear();
}

// Register a new driver
void DriverManager::registerDriver(boost::shared_ptr< ObjectInstancer<AbstractDriver> > instancer,
													  boost::shared_ptr< PluginInspector > description) throw(chaos::CException) {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
	
	if(!instancer)
		throw chaos::CException(1, "The instancer of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	if(!description)
		throw chaos::CException(2, "The description of the driver is mandatory for his registration", "DriverManager::registerDriver");
	
	std::string composedDriverName = description->getName();
	composedDriverName.append(description->getVersion());
	DMLAPP_ << "Register new driver with alias = \"" << composedDriverName<<"\"";
	
	if(mapDriverAliasVersionInstancer.count(composedDriverName)) {
		DMLAPP_ << "Driver \""<<composedDriverName<<"\" is already registered";
		return;
	}
	
	boost::shared_ptr< DriverPluginInfo > driver_plugin_info(new DriverPluginInfo());
	driver_plugin_info->sp_inspector = description;
	driver_plugin_info->sp_instancer = instancer;
	mapDriverAliasVersionInstancer.insert(make_pair(composedDriverName, driver_plugin_info));
}

// Get a new driver accessor for a driver instance
DriverAccessor *DriverManager::getNewAccessorForDriverInstance(DrvRequestInfo& request_info) throw(chaos::CException) {
	boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
	
	std::string composedDriverName;
	std::string stringForMap;
	DriverAccessor *accessor = NULL;
    std::string driverInfo;
	// the hashing of th einstance  is composed by name+version+input_parameter[if given], because
	// different device drive can have same parameter names. If no input is passed the hashing is
	// calculated using only the device name and the version.
	
	//befor hashing we need to check if the driver with alias and version has been registered
	DMLDBG_ << "Has been requested a driver with the information -> " << request_info.alias << "/" << request_info.version << "/" << request_info.init_parameter;
	composedDriverName = request_info.alias;
	composedDriverName.append(request_info.version);
	
	stringForMap = composedDriverName;
	stringForMap.append(request_info.init_parameter);
	driverInfo = std::string("\"") + std::string(request_info.alias) + std::string("\" ver=")+std::string(request_info.version);
    
	DMLDBG_ << "The identification string for the driver is \"" << stringForMap<<"\"";
	
	if(mapParameterLiveInstance.count(stringForMap)) {
		DMLDBG_ << "Driver \""<<stringForMap<<"\" is already been instantiated";
		
		if(mapParameterLiveInstance[stringForMap]->getNewAccessor(&accessor)) {
			//new accessor has been allocated
			DMLAPP_ << "Retrive driver accessor with index ="<< accessor->accessor_index << " for driver "<<driverInfo<<" with uuid =" << mapParameterLiveInstance[stringForMap];
			
			return accessor;
		} else {
			throw chaos::CException(1, "Device driver retriving accessor", "DriverManager::getNewAccessorForDriverInstance");
		}
	}
	DMLDBG_ << "Driver need to be instantiated using alias =\"" << stringForMap << "\" instancer presence = " << mapDriverAliasVersionInstancer.count(composedDriverName);
	//the instance of the driver need to be created
	if(mapDriverAliasVersionInstancer.count(composedDriverName)==0){
        std::map<std::string, boost::shared_ptr< DriverPluginInfo > >::iterator i;
        DMLDBG_ << "Driver "<<driverInfo<<" not found, here a list of possible driver names and versions:";
        for(i=mapDriverAliasVersionInstancer.begin();i!=mapDriverAliasVersionInstancer.end();i++){
            DMLDBG_ << "\""<<(*i).first<<"\"";
        }
        std::string msg ="Driver " + driverInfo +" not found";
		throw chaos::CException(1,msg , "DriverManager::getNewAccessorForDriverInstance");
	}
	// i can create the instance
	AbstractDriver *driverInstance = mapDriverAliasVersionInstancer[composedDriverName]->sp_instancer->getInstance();
	//associate the driver identification string
	driverInstance->identificationString = stringForMap;
	//initialize the newly create instance
	DMLAPP_ << "Initializing device driver " << driverInfo <<", initialization parameters:\""<<request_info.init_parameter<<"\" with uuid = " << driverInstance->driverUUID;
	InizializableService::initImplementation(driverInstance, (void*)(request_info.init_parameter.c_str()), "AbstractDriver", "DriverManager::getNewAccessorForDriverInstance");
	
	//here the driver has been initializated and has been associated with the hash of the parameter
	DMLAPP_ << "Add device driver with hash = " << driverInstance->identificationString;
	mapParameterLiveInstance.insert(make_pair(stringForMap, driverInstance));
	mapDriverUUIDHashLiveInstance.insert(make_pair(driverInstance->driverUUID, driverInstance));
	//now can get new accessor
	if(driverInstance->getNewAccessor(&accessor)) {
		DMLAPP_ << "Got new driver accessor with index =\""<< accessor->accessor_index << "\" for driver:"<<driverInfo<<" driver with uuid =" << driverInstance->driverUUID;
	}
	return accessor;
}

//! release the accessor instance
void DriverManager::releaseAccessor(DriverAccessor *accessor) {
	boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
	
	if(!mapDriverUUIDHashLiveInstance.count(accessor->driver_uuid)) return;
	
	AbstractDriver *dInstance = mapDriverUUIDHashLiveInstance[accessor->driver_uuid];
	//we can relase the accessor
	if(!dInstance->releaseAccessor(accessor)) {
		throw chaos::CException(1, "This errore never have to appen", "DriverManager::releaseAccessor");
	}
	if(!dInstance->accessors.size()) {
		DMLAPP_ << "The driver with uuid =" << dInstance->driverUUID << " has no more accessor allocated so we can deinitialize it";
		//deinitialize driver
		try {
			DMLAPP_ << "Deinitializing device driver with uuid = " << dInstance->driverUUID <<" (\""<<dInstance->identificationString + "\")";
			InizializableService::deinitImplementation(dInstance, "AbstractDriver", "DriverManager::deinit");
		} catch (...) {
			DMLAPP_ << "Error deinitializing device driver with uuid = " << dInstance->driverUUID<<" (\""<<dInstance->identificationString + "\")";
			
		}
		DMLAPP_ << "Deleting device driver with uuid = " << dInstance->driverUUID;
		mapParameterLiveInstance.erase(dInstance->identificationString);
		mapDriverUUIDHashLiveInstance.erase(dInstance->driverUUID);
		delete(dInstance);
	}
}