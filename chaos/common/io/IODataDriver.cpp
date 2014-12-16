//
//  IODataDriver.cpp
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 12/05/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/io/QueryFuture.h>

#define IODataDriverLOG_HEAD "[IODataDriver] - "

#define IODataDriverLAPP	LAPP_ << IODataDriverLOG_HEAD
#define IODataDriverLDBG	LDBG_ << IODataDriverLOG_HEAD << __PRETTY_FUNCTION__ << " - "
#define IODataDriverLERR	LERR_ << IODataDriverLOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

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
void IODataDriver::storeData(const std::string& key,
							 CDataWrapper *dataToStore,
							 int store_hint) throw(CException){
    
    if(!dataToStore) return;
	
	SerializationBuffer* serialization = dataToStore->getBSONData();

	storeRawData(key, serialization, store_hint);
	
	delete(dataToStore);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ArrayPointer<CDataWrapper>*  IODataDriver::retriveData(const std::string& key, CDataWrapper*const)  throw(CException){
        //check for key length
    return retriveData(key);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ArrayPointer<CDataWrapper>* IODataDriver::retriveData(const std::string& key)  throw(CException) {
    
    ArrayPointer<CDataWrapper> *result = new ArrayPointer<CDataWrapper>();
    
    char *value = retriveRawData(key);
    if (value) {
            //some value has been received
            //allocate the data wrapper object with serialization got from memcached
            //CDataWrapper *dataWrapper = 
        result->add(new CDataWrapper(value));
        free(value);
    }
    return result;
}

//! restore all the datasets for a restore point
int IODataDriver::loadRestorePoint(const std::string& restore_point_tag_name,
								   const std::vector<std::string> key_list) {
	int err = 0;
	chaos_data::CDataWrapper *dataset = NULL;
	
	if(!map_restore_point.count(restore_point_tag_name)) {
		//initialize resto point datasets map
		map_restore_point.insert(make_pair(restore_point_tag_name, std::map<std::string, boost::shared_ptr<chaos_data::CDataWrapper> >()));
	}
	
	for (std::vector<std::string>::const_iterator it = key_list.begin();
		 it != key_list.end();
		 it++) {
		if((err = loadDatasetToRestorePoint(restore_point_tag_name, *it, &dataset))) {
			IODataDriverLERR << "Error loading dataset form restore point with code:" << err;
			break;
		} else {
			if(dataset) {
				//! put found dataset on restore point hash
				map_restore_point[restore_point_tag_name].insert(make_pair(*it, boost::shared_ptr<chaos_data::CDataWrapper>(dataset)));
			}
			dataset = NULL;
		}
	}
	return err;
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
boost::shared_ptr<chaos_data::CDataWrapper> IODataDriver::getDatasetFromRestorePoint(const std::string& restore_point_tag_name,
																   const std::string& dataset_key) {
	if(!map_restore_point.count(restore_point_tag_name)) return boost::shared_ptr<chaos_data::CDataWrapper>();
	
	if(!map_restore_point[restore_point_tag_name].count(dataset_key)) return boost::shared_ptr<chaos_data::CDataWrapper>();
	
	return map_restore_point[restore_point_tag_name][dataset_key];
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
void IODataDriver::_pushResultToQueryFuture(QueryFuture& query_future,
											chaos_data::CDataWrapper *data_pack,
											uint64_t element_index) {
	query_future.pushDataPack(data_pack, element_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_startQueryFutureResult(QueryFuture& query_future,
										   uint64_t _total_element_found) {
	query_future.notifyStartResultPhase(_total_element_found);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::_endQueryFutureResult(QueryFuture& query_future,
										 int32_t _error,
										 const std::string& _error_message) {
	query_future.notifyEndResultPhase(_error, _error_message);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryFuture *IODataDriver::performQuery(const std::string& key,
										uint64_t start_ts,
										uint64_t end_ts) {
	return NULL;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::releaseQuery(QueryFuture *query_future) {
	_releaseQueryFuture(query_future);
	query_future = NULL;
}