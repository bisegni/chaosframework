//
//  IODataDriver.cpp
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 12/05/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <chaos/common/io/IODataDriver.h>

using namespace chaos;
using namespace chaos::common::data;

/*
 * Init method, the has map has all received value for configuration
 * every implemented driver need to get all needed configuration param
 */
 void IODataDriver::init(void *init_parameter) throw(CException){
    
}
/*
 * DeInit method
 */
void IODataDriver::deinit() throw(CException) {
    
}

/*
 * This method cache all object passed to driver
 */
void IODataDriver::storeData(CDataWrapper *dataToStore) throw(CException){
    
    if(!dataToStore) return;
    
        //get the key to store data on the memcached
        //string key = dataToStore->getStringValue(DataPackKey::CS_CSV_CU_ID);
	SerializationBuffer* serialization = dataToStore->getBSONData();
    //if(!serialization) {
   //     return;
   // }
    
    storeRawData(serialization);
   // delete(serialization);

}


/*
 * This method retrive the cached object by CSDawrapperUsed as query key and
 * return a pointer to the class ArrayPointer of CDataWrapper type
 */
ArrayPointer<CDataWrapper>*  IODataDriver::retriveData(CDataWrapper*const)  throw(CException){
        //check for key length
    return retriveData();
}

/*
 * This method retrive the cached object by CSDawrapperUsed as query key and
 * return a pointer to the class ArrayPointer of CDataWrapper type
 */
ArrayPointer<CDataWrapper>* IODataDriver::retriveData()  throw(CException) {
    
    ArrayPointer<CDataWrapper> *result = new ArrayPointer<CDataWrapper>();
    
    char *value = retriveRawData();
    if (value) {
            //some value has been received
            //allocate the data wrapper object with serialization got from memcached
            //CDataWrapper *dataWrapper = 
        result->add(new CDataWrapper(value));
        free(value);
    }
    return result;
}

/*
 Update the driver configuration
 */
CDataWrapper* IODataDriver::updateConfiguration(CDataWrapper*){
    return NULL;
}