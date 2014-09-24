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

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
 void IODataDriver::init(void *init_parameter) throw(CException){
    
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::deinit() throw(CException) {
    
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
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


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ArrayPointer<CDataWrapper>*  IODataDriver::retriveData(CDataWrapper*const)  throw(CException){
        //check for key length
    return retriveData();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
CDataWrapper* IODataDriver::updateConfiguration(CDataWrapper*){
    return NULL;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryFuture *IODataDriver::_getNewQueryFutureForQueryID(const std::string& query_id) {
	//allcoate the query future
	QueryFuture *result = new QueryFuture(query_id);
	
	//se tthe state
	if(result)result->state = QueryFutureStateCreated;
	return result;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_releaseQueryFuture(QueryFuture *query_future_ptr) {
	if(query_future_ptr)delete(query_future_ptr);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_pushResultToQueryFuture(QueryFuture& query_future, chaos_data::CDataWrapper *data_pack, uint64_t element_index) {
	query_future.pushDataPack(data_pack, element_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_startQueryFutureResult(QueryFuture& query_future, uint64_t _total_element_found) {
	query_future.notifyStartResultPhase(_total_element_found);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_endQueryFutureResult(QueryFuture& query_future, int32_t _error, const std::string& _error_message) {
	query_future.notifyEndResultPhase(_error, _error_message);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryFuture *IODataDriver::performQuery(uint64_t start_ts, uint64_t end_ts) {
	return NULL;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::releaseQuery(QueryFuture *query_future) {
	_releaseQueryFuture(query_future);
	query_future = NULL;
}