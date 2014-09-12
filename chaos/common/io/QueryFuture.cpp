//
//  QueryFuture.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 11/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "QueryFuture.h"

using namespace chaos::common::io;

QueryFuture::QueryFuture(const std::string& _query_id):
query_id(_query_id),
total_found_element(0),
fetched_element_index(0),
data_pack_queue(1){
	
}

QueryFuture::~QueryFuture() {
	
}

const std::string& QueryFuture::getQueryID() {
	return query_id;
}

void QueryFuture::pushDataPack(cc_data::CDataWrapper *received_datapack, uint64_t _total_found_element) {
	total_found_element = _total_found_element;
	fetched_element_index++;
	
	while(!data_pack_queue.push(received_datapack)) {
		waith_for_push_data_Semaphore.wait();
	}
	waith_for_get_data_Semaphore.unlock();
}

cc_data::CDataWrapper *QueryFuture::getDataPack(bool wait) {
	cc_data::CDataWrapper *result = NULL;
	while(!data_pack_queue.pop(result) && wait) {
		waith_for_get_data_Semaphore.unlock();
	}
	return result;
}

uint64_t QueryFuture::getTotalElementFound() {
	return total_found_element;
}

uint64_t QueryFuture::getCurrentElementIndex() {
	return fetched_element_index;
}