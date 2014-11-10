/*	
 *	KeyDataStorage.cpp
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

#include "KeyDataStorage.h"
#include "../../common/global.h"
#include "../../common/chaos_constants.h"


using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::data_manager;

#define DEFAULT_OBUFFER_THREAD_NUMEBR   1

KeyDataStorage::KeyDataStorage(const std::string& _key,
							   chaos_io::IODataDriver *_io_data_driver):
key(_key),
io_data_driver(_io_data_driver) {
	output_key	= _key + DataPackPrefixID::OUTPUT_DATASE_PREFIX;
	input_key	= _key + DataPackPrefixID::INPUT_DATASE_PREFIX;
	system_key	= _key + DataPackPrefixID::SYSTEM_DATASE_PREFIX;
	custom_key	= _key + DataPackPrefixID::CUSTOM_DATASE_PREFIX;
}

KeyDataStorage::~KeyDataStorage() {
    
}

void KeyDataStorage::init(void *init_parameter) throw (chaos::CException) {
	if(!io_data_driver) throw chaos::CException(-1, "IO data driver not set", __PRETTY_FUNCTION__);
	
	io_data_driver->init(init_parameter);
}

void KeyDataStorage::deinit() throw (chaos::CException) {
	if(io_data_driver) io_data_driver->deinit();
}
/*
 Return a new instace for the CSDatawrapped filled
 with default madatory data
 */
CDataWrapper* KeyDataStorage::getNewOutputAttributeDataWrapper() {
    CDataWrapper *result = new CDataWrapper();
        //add key to outputdata wrapper
    result->addStringValue(DataPackKey::CS_CSV_CU_ID, key);
    return result;
}

/*
 Retrive the data from Live Storage
 */
ArrayPointer<CDataWrapper>* KeyDataStorage::getLastDataSet(KeyDataStorageDomain domain) {
        //retrive data from cache for the key managed by
        //this instance of keydatastorage
	CHAOS_ASSERT(io_data_driver);
	//lock for protect the access
	boost::unique_lock<boost::mutex> l(mutex_push_data);
	switch(domain) {
		case KeyDataStorageDomainOutput:
			  return io_data_driver->retriveData(output_key);
			break;
		case KeyDataStorageDomainInput:
			  return io_data_driver->retriveData(input_key);
			break;
		case KeyDataStorageDomainSystem:
			  return io_data_driver->retriveData(system_key);
			break;
		case KeyDataStorageDomainCustom:
			  return io_data_driver->retriveData(custom_key);
			break;
	}
	
}

void KeyDataStorage::pushDataSet(KeyDataStorageDomain domain, chaos_data::CDataWrapper *dataset) {
	CHAOS_ASSERT(io_data_driver);
	//lock for protect the access
	boost::unique_lock<boost::mutex> l(mutex_push_data);
	switch(domain) {
		case KeyDataStorageDomainOutput:
			io_data_driver->storeData(output_key, dataset);
			break;
		case KeyDataStorageDomainInput:
			io_data_driver->storeData(input_key, dataset);
			break;
		case KeyDataStorageDomainSystem:
			io_data_driver->storeData(system_key, dataset);
			break;
		case KeyDataStorageDomainCustom:
			io_data_driver->storeData(custom_key, dataset);
			break;
	}

}


/*
 Retrive the data from History Storage
 */
ArrayPointer<CDataWrapper>* KeyDataStorage::getHistoricalDataSet(CDataWrapper *searchIption) {
    return NULL;
}

/*
 
 */
CDataWrapper* KeyDataStorage::updateConfiguration(CDataWrapper *newConfiguration) {
        //add in the configuration the key for the device
    newConfiguration->addStringValue(DataProxyConfigurationKey::CS_DM_LD_CU_ADDRESS_KEY, key);
		//update the driver configration
	if(io_data_driver) io_data_driver->updateConfiguration(newConfiguration);
    return NULL;
}
