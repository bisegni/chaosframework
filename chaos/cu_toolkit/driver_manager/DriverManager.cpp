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

#include <chaos/cu_toolkit/driver_manager/DriverManager.h>

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::common::plugin;
using namespace chaos::common::utility;

using namespace chaos::cu::driver_manager;
using namespace chaos::cu::driver_manager::driver;

#define DMLAPP_   INFO_LOG(DriverManager)
#define DMLDBG_   DBG_LOG(DriverManager)
#define DMLERR_   ERR_LOG(DriverManager)

#define INIT_HARDWARE_PARAM     "driver_param"
#define INIT_DEVICE_PARAM       "device_param"

DriverManager::DriverManager() {}
DriverManager::~DriverManager() {}

void DriverManager::init(void *initParameter)  {}
void DriverManager::start()  {}
void DriverManager::stop()  {}
void DriverManager::deinit()  {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
    
    //deinitialize all allcoated driver
    mapParameterLiveInstance.clear();
    for (std::map<std::string,
         AbstractDriver *>::iterator it = mapDriverUUIDHashLiveInstance.begin();
         it != mapDriverUUIDHashLiveInstance.end();
         it++) {
        
        //deinitialize driver
        try {
            DMLAPP_ << "Deinitializing device driver with uuid = " << it->second->driver_uuid;
            InizializableService::deinitImplementation(it->second, "AbstractDriver", "DriverManager::deinit");
        } catch (...) {
            DMLAPP_ << "Error deinitializing device driver with uuid = " << it->second->driver_uuid;
            
        }
        DMLAPP_ << "Deleting device driver with uuid = " << it->second->driver_uuid;
        delete (it->second);
    }
    mapDriverUUIDHashLiveInstance.clear();
    mapParameterLiveInstance.clear();
}

void DriverManager::registerDriver(ChaosSharedPtr<ObjectInstancer<AbstractDriver> > instancer,
                                   ChaosSharedPtr<PluginInspector> description)  {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
    
    if (!instancer) {
        throw chaos::CException(1, "The instancer of the driver is mandatory for his registration",
                                "DriverManager::registerDriver");
    }
    
    if (!description) {
        throw chaos::CException(2, "The description of the driver is mandatory for his registration",
                                "DriverManager::registerDriver");
    }
    
    std::string composedDriverName = description->getName();
    composedDriverName.append(description->getVersion());
    DMLAPP_ << "Register new driver with alias = \"" << composedDriverName << "\"";
    
    if (mapDriverAliasVersionInstancer.count(composedDriverName)) {
        DMLAPP_ << "Driver \"" << composedDriverName << "\" is already registered";
        return;
    }
    
    ChaosSharedPtr<DriverPluginInfo> driver_plugin_info(new DriverPluginInfo());
    driver_plugin_info->sp_inspector = description;
    driver_plugin_info->sp_instancer = instancer;
    mapDriverAliasVersionInstancer.insert(make_pair(composedDriverName, driver_plugin_info));
}

DriverAccessor *DriverManager::getNewAccessorForDriverInstance(DrvRequestInfo &request_info)  {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
    
    std::string composedDriverName;
    std::string stringForMap;
    DriverAccessor *accessor = NULL;
    std::string driverInfo;
    // the hashing of the instance  is composed by name+version+input_parameter[if given], because
    // different device drive can have same parameter names. If no input is passed the hashing is
    // calculated using only the device name and the version.
    
    //befor hashing we need to check if the driver with alias and version has been registered
    
    DMLDBG_ << "Has been requested a driver with the information -> " << request_info.alias << "/" << request_info.version << "/" << request_info.init_parameter;
    composedDriverName = request_info.alias;
    composedDriverName.append(request_info.version);
    
    stringForMap = composedDriverName;
    
    //try to indeitfy the json init parameter format
    CDWUniquePtr json_param = CDataWrapper::instanceFromJson(request_info.init_parameter);
    if(!json_param->isEmpty() &&
       json_param->hasKey(INIT_HARDWARE_PARAM) &&
       json_param->isCDataWrapperValue(INIT_HARDWARE_PARAM)) {
        stringForMap.append(json_param->getCSDataValue(INIT_HARDWARE_PARAM)->getJSONString());
    } else {
        stringForMap.append(request_info.init_parameter);
    }
    driverInfo = boost::str(boost::format("\"%1%\" ver=%2%")%request_info.alias%request_info.version);
    
    DMLDBG_ << "The identification string for the driver is \"" << stringForMap << "\"";
    if (mapParameterLiveInstance.count(stringForMap)) {
        DMLDBG_ << "Driver \"" << stringForMap << "\" is already been instantiated";
        
        if (mapParameterLiveInstance[stringForMap]->getNewAccessor(&accessor)) {
            //new accessor has been allocated
            DMLAPP_ << "Retrieve driver accessor with index =" << accessor->accessor_index << " for driver " << driverInfo << " with uuid =" << mapParameterLiveInstance[stringForMap];
            //check if the driver confguration has the opcode parameter
            if(accessor &&
               !json_param->isEmpty() &&
               json_param->hasKey(INIT_DEVICE_PARAM) &&
               json_param->isCDataWrapperValue(INIT_DEVICE_PARAM)) {
                //driver_init_paramter is private and we are friend of accessor class
                accessor->device_param = json_param->getCSDataValue(INIT_DEVICE_PARAM);
            }
            return accessor;
        } else {
            throw chaos::CException(1, "Device driver retrieving accessor", "DriverManager::getNewAccessorForDriverInstance");
        }
    }
    DMLDBG_ << "Driver need to be instantiated using alias =\"" << stringForMap << "\" instancer presence = " << mapDriverAliasVersionInstancer.count(
                                                                                                                                                      composedDriverName);
    //the instance of the driver need to be created
    if (mapDriverAliasVersionInstancer.count(composedDriverName) == 0) {
        std::map<std::string,
        ChaosSharedPtr<DriverPluginInfo> >::iterator i;
        DMLDBG_ << boost::str(boost::format("Driver %1% not found, here a list of possible driver names and versions")%driverInfo);
        for (i = mapDriverAliasVersionInstancer.begin();
             i != mapDriverAliasVersionInstancer.end();
             i++) {
            DMLDBG_ << "\"" << (*i).first << "\"";
        }
        throw chaos::CException(1, boost::str(boost::format("Driver %1% not found")%driverInfo), "DriverManager::getNewAccessorForDriverInstance");
    }
    // i can create the instance
    AbstractDriver *driverInstance = mapDriverAliasVersionInstancer[composedDriverName]->sp_instancer->getInstance();
    //associate the driver identification string
    driverInstance->identification_string = stringForMap;
    //initialize the newly create instance
    DMLAPP_ << "Initializing device driver " << driverInfo << ", initialization parameters:\"" << request_info.init_parameter << "\" with uuid = " << driverInstance->driver_uuid;
    InizializableService::initImplementation(driverInstance,
                                             (void *) (request_info.init_parameter.c_str()),
                                             "AbstractDriver",
                                             "DriverManager::getNewAccessorForDriverInstance");
    
    //here the driver has been initialized and has been associated with the hash of the parameter
    DMLAPP_ << "Add device driver with hash = " << driverInstance->identification_string;
    mapParameterLiveInstance.insert(make_pair(stringForMap, driverInstance));
    mapDriverUUIDHashLiveInstance.insert(make_pair(driverInstance->driver_uuid, driverInstance));
    //now can get new accessor
    if (driverInstance->getNewAccessor(&accessor)) {
        DMLAPP_ << "Got new driver accessor with index =\"" << accessor->accessor_index << "\" for driver:" << driverInfo << " driver with uuid =" << driverInstance->driver_uuid;
    }
    
    //check if the driver confguration has the opcode parameter
    if(accessor &&
       !json_param->isEmpty() &&
       json_param->hasKey(INIT_DEVICE_PARAM) &&
       json_param->isCDataWrapperValue(INIT_DEVICE_PARAM)) {
        //driver_init_paramter is private and we are friend of accessor class
        accessor->device_param = json_param->getCSDataValue(INIT_DEVICE_PARAM);
    }
    return accessor;
}

void DriverManager::releaseAccessor(DriverAccessor *accessor) {
    boost::unique_lock<boost::shared_mutex> lock(mutextMapAccess);
    
    if (!mapDriverUUIDHashLiveInstance.count(accessor->driver_uuid)) {
        return;
    }
    
    AbstractDriver *dInstance = mapDriverUUIDHashLiveInstance[accessor->driver_uuid];
    //we can relase the accessor
    if (!dInstance->releaseAccessor(accessor)) {
        throw chaos::CException(1, "This errore never have to appen", "DriverManager::releaseAccessor");
    }
    if (!dInstance->accessors.size()) {
        DMLAPP_ << "The driver with uuid =" << dInstance->driver_uuid << " has no more accessor allocated so we can deinitialize it";
        //deinitialize driver
        try {
            DMLAPP_ << "Deinitializing device driver with uuid = " << dInstance->driver_uuid << " (\"" << dInstance->identification_string<<"\")";
            InizializableService::deinitImplementation(dInstance, "AbstractDriver", "DriverManager::deinit");
        } catch (...) {
            DMLAPP_ << "Error deinitializing device driver with uuid = " << dInstance->driver_uuid << " (\"" << dInstance->identification_string << "\")";
            
        }
        DMLAPP_ << "Deleting device driver with uuid = " << dInstance->driver_uuid;
        mapParameterLiveInstance.erase(dInstance->identification_string);
        mapDriverUUIDHashLiveInstance.erase(dInstance->driver_uuid);
        delete (dInstance);
    }
}
