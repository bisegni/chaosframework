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
data_pack_queue(1){
	
}

QueryFuture::~QueryFuture() {
	
}

const std::string& QueryFuture::getQueryID() {
	return query_id;
}

void QueryFuture::pushDataPack(cc_data::CDataWrapper *received_datapack) {
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