//
//  QueryFuture.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 11/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "QueryFuture.h"

using namespace chaos::common::io;

#define QueryFuture_LOG_HEAD "[QueryFuture] "

#define QFLAPP_ LAPP_ << QueryFuture_LOG_HEAD << "-"
#define QFLDBG_ LDBG_ << QueryFuture_LOG_HEAD << __PRETTY_FUNCTION__ << " - "
#define QFLERR_ LERR_ << QueryFuture_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ")" << "-"


QueryFuture::QueryFuture(const std::string& _query_id):
query_id(_query_id),
total_found_element(0),
fetched_element_index(0),
error(0),
error_message(""),
state(QueryFutureStateCreated),
data_pack_queue(1){
	
}

QueryFuture::~QueryFuture() {
	
}

const std::string& QueryFuture::getQueryID() {
	return query_id;
}

void QueryFuture::pushDataPack(cc_data::CDataWrapper *received_datapack, uint64_t _datapack_index) {
	fetched_element_index++;
	state = QueryFutureStateReceivingResult;
	while(!data_pack_queue.push(received_datapack)) {
		waith_for_push_data_Semaphore.wait();
	}
	waith_for_get_data_Semaphore.unlock();
}

void QueryFuture::notifyStartResultPhase(uint64_t _total_element_found) {
	QFLDBG_ << "Received start query phase result wiht element found:" << _total_element_found;
	state = QueryFutureStateStartResult;
	total_found_element = _total_element_found;
	waith_for_begin_result.unlock();
}

void QueryFuture::notifyEndResultPhase(int32_t _error, const std::string& _error_message) {
	QFLDBG_ << "Received error:" << _error << " and message:" << _error_message;
	error = _error;
	if(error) {
		state = QueryFutureStateError;
		error_message = _error_message;
	} else {
		state = QueryFutureStateEndResult;
	}
	//unlock all
	waith_for_begin_result.unlock();
	waith_for_get_data_Semaphore.unlock();
}

void QueryFuture::waitForBeginResult(int32_t timeout) {
	DEBUG_CODE(QFLDBG_ << "Waiting for begin result";)
	if(state != QueryFutureStateCreated) return;
	if(timeout < 0) {
		waith_for_begin_result.wait();
	}else{
		waith_for_begin_result.wait(timeout);
	}
	DEBUG_CODE(QFLDBG_ << "End waiting result";)
}

cc_data::CDataWrapper *QueryFuture::getDataPack(bool wait, uint32_t timeout) {
	cc_data::CDataWrapper *result = NULL;
	if(timeout) {
		//wait only once an the return
		DEBUG_CODE(QFLDBG_ << "waiting result for " << timeout << " milliseconds";)
		if(!data_pack_queue.pop(result) && wait) {
			DEBUG_CODE(QFLDBG_ << "got new datapack";)
			//we have no data wait the timeout to receck
			waith_for_get_data_Semaphore.wait(timeout);
			
			//try to get next
			data_pack_queue.pop(result);
		} else {
            DEBUG_CODE(if(!result){QFLDBG_ << "no data received";})
		}
	}else{
		//cicle and waith untile we have a data
		DEBUG_CODE(QFLDBG_ << "Waiting result with no timeout";)
		while(!data_pack_queue.pop(result) && wait) {
			DEBUG_CODE(QFLDBG_ << "Got result";)
			waith_for_get_data_Semaphore.wait();
		}
	}
	waith_for_get_data_Semaphore.unlock();
	return result;
}

uint64_t QueryFuture::getTotalElementFound() {
	return total_found_element;
}

uint64_t QueryFuture::getCurrentElementIndex() {
	return fetched_element_index;
}

int32_t QueryFuture::getError() {
	return error;
}

const std::string& QueryFuture::getErrorMessage() {
	return error_message;
}

QueryFutureState QueryFuture::getState() {
	return state;
}