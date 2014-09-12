//
//  IODataDriver.cpp
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 12/05/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/io/QueryFuture.h>

using namespace chaos;
using namespace chaos::common::io;
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

QueryFuture *IODataDriver::_getNewQueryFutureForQueryID(const std::string& query_id) {
	return new QueryFuture(query_id);
}

void IODataDriver::_releaseQueryFuture(QueryFuture *query_future_ptr) {
	if(query_future_ptr)delete(query_future_ptr);
}

void IODataDriver::_pushDataToQuryFuture(QueryFuture& query_future, chaos_data::CDataWrapper *data_pack, uint64_t total_element_found) {
	query_future.pushDataPack(data_pack, total_element_found);
}

QueryFuture *IODataDriver::performQuery(uint64_t start_ts, uint64_t end_ts) {
	return NULL;
}

//! close a query
void IODataDriver::releaseQuery(QueryFuture *query_future) {
	_releaseQueryFuture(query_future);
}