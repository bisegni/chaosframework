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

#include <iostream>
#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos/common/io/IODirectIODriver.h>

#include <chaos/cu_toolkit/data_manager/DataManager.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>

#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::cu::data_manager;
using namespace chaos::common::data;


DataManager::DataManager() {}

DataManager::~DataManager() {}


void DataManager::init(void *initParameter) {
    LAPP_ << "Starting Data Manager";
}

void DataManager::deinit() {
    ChaosUniquePtr<KeyDataStorage> tmpKDS;
    map<string, KeyDataStorage*>::iterator iter = deviceIDKeyDataStorageMap.begin();
    
    for (  ; iter != deviceIDKeyDataStorageMap.end(); iter++ ) {
        LAPP_ << "Deinitializating KeyDataStorage for key:" << (*iter).first;
        tmpKDS.reset((*iter).second);
        tmpKDS->deinit();
    }
        //remove all
    deviceIDKeyDataStorageMap.clear();
}

void DataManager::start() {}

CDataWrapper* DataManager::updateConfiguration(CDataWrapper *newConfiguration) {
    LAPP_ << "Update Configuraiton for DataManager";
    map<string, KeyDataStorage*>::iterator iter = deviceIDKeyDataStorageMap.begin();
    
    KeyDataStorage *tmpKDS = 0L;
        //update the Contorl Unit's KeyDataStorage
    for (  ; iter != deviceIDKeyDataStorageMap.end(); iter++ ) {
        LAPP_ << "Update configuraiton for KeyDataStorage for key:" << (*iter).first;
        tmpKDS = (*iter).second;
        tmpKDS->updateConfiguration(newConfiguration);
    }
    return NULL;
}

IODataDriver *DataManager::getDataLiveDriverNewInstance() {
	IODataDriver *result = NULL;
	std::string impl_name =  boost::str( boost::format("%1%IODriver") % GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
	
	result = common::utility::ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(impl_name);
	
	if(result) {
		if(impl_name.compare("IODirectIODriver") == 0) {
			//set the information
			IODirectIODriverInitParam init_param;
			std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
			//get client and endpointb
			init_param.client_instance = NULL;
			init_param.endpoint_instance = NULL;
			((IODirectIODriver*)result)->setDirectIOParam(init_param);
		}
	}
    return result;
}

KeyDataStorage *DataManager::getKeyDataStorageNewInstanceForKey(const string& key) {
    IODataDriver *outputDriver = getDataLiveDriverNewInstance();
    return new KeyDataStorage(key, outputDriver);
}

void DataManager::initDeviceIDKeyDataStorage(const string& device_id, CDataWrapper *initializationParameter) {
    if(deviceIDKeyDataStorageMap.count(device_id) > 0 ) return;
    
    LAPP_ << "Adding a new KeyDataStorage for device id key:" << device_id;
        //need to be create the KeyDataStorage
    KeyDataStorage *kds = getKeyDataStorageNewInstanceForKey(device_id);
        //initiliaze the KeyDataStorage
    kds->init(initializationParameter);
        //add the data storage to map
    deviceIDKeyDataStorageMap.insert(make_pair(device_id, kds));
}

void DataManager::deinitDeviceIDKeyDataStorage(const string& device_id)  {
    LAPP_ << "Deinit and Delete KeyDataStorage for device id key:" << device_id;
        //try to find the device id key

    map<string, KeyDataStorage*>::iterator iter = deviceIDKeyDataStorageMap.find(device_id);
        
    if(iter == deviceIDKeyDataStorageMap.end()) return;
        
            //get the pointer
    ChaosUniquePtr<KeyDataStorage> tmpKDS((*iter).second);
    LAPP_ << "Remove in the manager the KeyDataStorage for device id key:" << device_id;
            //remove key from map
    deviceIDKeyDataStorageMap.erase(device_id);
        
            // the buffer has been found
    tmpKDS->deinit();
}

void DataManager::updateConfigurationForDeviceIdKey(const string& device_id, CDataWrapper* newConfiguration) {
    deviceIDKeyDataStorageMap[device_id]->updateConfiguration(newConfiguration);
}

void DataManager::pushDeviceDataByIdKey(const string& device_id, CDWShrdPtr dataset) {
        deviceIDKeyDataStorageMap[device_id]->pushDataSet(data_manager::KeyDataStorageDomainOutput, MOVE(dataset));
}

CDWShrdPtr DataManager::getNewDataWrapperForDeviceIdKey(const string& device_id) {
    return deviceIDKeyDataStorageMap[device_id]->getNewDataPackForDomain(KeyDataStorageDomainOutput);
}
