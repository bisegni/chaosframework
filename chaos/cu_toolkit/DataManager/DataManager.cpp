/*	
 *	DataManager.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <iostream>
#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos/common/io/IOMemcachedIODriver.h>
#include <chaos/common/io/IODirectIODriver.h>

#include <chaos/cu_toolkit/DataManager/DataManager.h>
#include <chaos/cu_toolkit/DataManager/DataBuffer/OutputDataBuffer.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>

#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::cu;
using namespace chaos::common::data;

/*
 * 
 */
DataManager::DataManager() {
        //liveDriver = 0L;
}

/*
 * 
 */
DataManager::~DataManager() {
        //if(liveDriver) delete(liveDriver);
}

/*
 * Initzialize the datamanager
 */
void DataManager::init(void *initParameter) throw(CException) {
    LAPP_ << "Starting Data Manager";
}

/*
 * Deinitzialize the datamanager
 */
void DataManager::deinit() throw(CException) {
    auto_ptr<KeyDataStorage> tmpKDS;
    map<string, KeyDataStorage*>::iterator iter = deviceIDKeyDataStorageMap.begin();
    
    for (  ; iter != deviceIDKeyDataStorageMap.end(); iter++ ) {
        LAPP_ << "Deinitializating KeyDataStorage for key:" << (*iter).first;
        tmpKDS.reset((*iter).second);
        tmpKDS->deinit();
    }
        //remove all
    deviceIDKeyDataStorageMap.clear();
}

/*
 * Start all sub process
 */
void DataManager::start() throw(CException) {
    
}

/*
 Configure the sandbox and all subtree of the CU
 */
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

/*
 * Return an instance for the configured data live driver
 */
IODataDriver *DataManager::getDataLiveDriverNewInstance() throw(CException) {
	IODataDriver *result = NULL;
	std::string impl_name =  boost::str( boost::format("%1%IODriver") % GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
	
	result = ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(impl_name.c_str());
	
	if(result) {
		if(impl_name.compare("IODirectIODriver") == 0) {
			//set the information
			IODirectIODriverInitParam init_param;
			//get client and endpoint
			init_param.client_instance = CommandManager::getInstance()->broker->getDirectIOClientInstance();
			init_param.endpoint_instance = CommandManager::getInstance()->broker->getDirectIOServerEndpoint();
			((IODirectIODriver*)result)->setDirectIOParam(init_param);
		}
	}
    return result;
}


/*
 * return a ne winstance of a MultiBufferDataStorage class
 */
KeyDataStorage *DataManager::getKeyDataStorageNewInstanceForKey(string& key) throw(CException) {
        //creat enew instance
    KeyDataStorage *keyBuffer = new KeyDataStorage(key);
    if(!keyBuffer) throw CException(0, "error creating instance of KeyDataStorage", "DataManager::getKeyDataStorageNewInstanceForKey");
    
        //set the new instance of output live driver buffer
    OutputDataBuffer *outputLiveBuffer = new OutputDataBuffer();
    
        //make the live driver
    IODataDriver *outputDriver = getDataLiveDriverNewInstance();
    
        //set the driver
    outputLiveBuffer->setIODriver(outputDriver);
        
        //set the first buffer
    keyBuffer->setLiveOBuffer(outputLiveBuffer);
    
        //initialize the buffer
        //(*msBuffer).init();
    
        //return the MultiBufferDataStorage
    return keyBuffer;
}

/*
 Initialize a device id KeyDataStorageBuffer
 */
void DataManager::initDeviceIDKeyDataStorage(string& deviceIdKey, CDataWrapper *initializationParameter) throw(CException) {
    if(deviceIDKeyDataStorageMap.count(deviceIdKey) > 0 ) return;
    
    LAPP_ << "Adding a new KeyDataStorage for device id key:" << deviceIdKey;
        //need to be create the KeyDataStorage
    KeyDataStorage *kds = getKeyDataStorageNewInstanceForKey(deviceIdKey);
        //initiliaze the KeyDataStorage
    kds->init(initializationParameter);
        //add the data storage to map
    deviceIDKeyDataStorageMap.insert(make_pair(deviceIdKey, kds));
}

/*
 Initialize a device id KeyDataStorageBuffer
 */
void DataManager::deinitDeviceIDKeyDataStorage(string& deviceIdKey) throw(CException)  {
    LAPP_ << "Deinit and Delete KeyDataStorage for device id key:" << deviceIdKey;
        //try to find the device id key

    map<string, KeyDataStorage*>::iterator iter = deviceIDKeyDataStorageMap.find(deviceIdKey);
        
    if(iter == deviceIDKeyDataStorageMap.end()) return;
        
            //get the pointer
    auto_ptr<KeyDataStorage> tmpKDS((*iter).second);
    LAPP_ << "Remove in the manager the KeyDataStorage for device id key:" << deviceIdKey;
            //remove key from map
    deviceIDKeyDataStorageMap.erase(deviceIdKey);
        
            // the buffer has been found
    tmpKDS->deinit();
}

/*
 Configure the sandbox and all subtree of the CU
 */
void DataManager::updateConfigurationForDeviceIdKey(string& deviceIdKey, CDataWrapper* newConfiguration) {
    deviceIDKeyDataStorageMap[deviceIdKey]->updateConfiguration(newConfiguration);
}

/*
 Submit a CDataWrapper on device id KeyDataStorage
 */
void DataManager::pushDeviceDataByIdKey(string& deviceIdKey, CDataWrapper* deviceCDataWrapper) throw(CException) {
        deviceIDKeyDataStorageMap[deviceIdKey]->pushDataSet(deviceCDataWrapper);
}

/*
 get last dataset for a specified key
 */
ArrayPointer<CDataWrapper> *DataManager::getLastCDataWrapperForDeviceIdKey(string& deviceIdKey)  throw(CException) {
        //use keydatastorage from map
    return deviceIDKeyDataStorageMap[deviceIdKey]->getLastDataSet();
}

/*
 return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *DataManager::getNewDataWrapperForDeviceIdKey(string& deviceIdKey) {
    return deviceIDKeyDataStorageMap[deviceIdKey]->getNewDataWrapper();
}
