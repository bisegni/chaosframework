/*
 *	SnapshotCreationWorker.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "../ChaosDataService.h"
#include "SnapshotCreationWorker.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::data_service::worker;

#define SnapshotCreationWorker_LOG_HEAD "[SnapshotCreationWorker] - "
#define SCW_LAPP_ LAPP_ << SnapshotCreationWorker_LOG_HEAD
#define SCW_LDBG_ LDBG_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - "
#define SCW_LERR_ LERR_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - " << __LINE__ << "-"


//------------------------------------------------------------------------------------------------------------------------

SnapshotCreationWorker::SnapshotCreationWorker(const std::string& _cache_impl_name,
											   db_system::DBDriver	*_db_driver_ptr,
											   NetworkBroker	*_network_broker):
cache_impl_name(_cache_impl_name),
cache_driver_ptr(NULL),
db_driver_ptr(_db_driver_ptr),
network_broker(_network_broker),
mds_channel(NULL){}

SnapshotCreationWorker::~SnapshotCreationWorker() {}

void SnapshotCreationWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	
	if(!network_broker) throw CException(-1, "No network broker set", __PRETTY_FUNCTION__);
	
	SCW_LAPP_ << "get mds channel";
	mds_channel = network_broker->getMetadataserverMessageChannel();
	if(!mds_channel) throw CException(-2, "No metadataserver channel created", __PRETTY_FUNCTION__);
	
	SCW_LAPP_ << "allocating cache driver";
	cache_driver_ptr = ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name);
	if(!cache_driver_ptr) throw chaos::CException(-3, "Cached driver not found", __PRETTY_FUNCTION__);
	InizializableService::initImplementation(cache_driver_ptr, &ChaosDataService::getInstance()->setting.cache_driver_setting, "CacheDriver", __PRETTY_FUNCTION__);
}

void SnapshotCreationWorker::deinit() throw (chaos::CException) {
	SCW_LAPP_ << "deallocating cache driver";
	if(cache_driver_ptr) {
		try{
			InizializableService::deinitImplementation(cache_driver_ptr, "CacheDriver", __PRETTY_FUNCTION__);
		} catch(...) {
		}
		delete(cache_driver_ptr);
	}
	SCW_LAPP_ << "deallocating db driver";
	
	if(mds_channel && network_broker) {
		network_broker->disposeMessageChannel(mds_channel);
	}
	
	DataWorker::deinit();
}

int SnapshotCreationWorker::submitJobInfo(WorkerJobPtr job_info) {
	int err = 0;
	SnapshotCreationJob *job_ptr = reinterpret_cast<SnapshotCreationJob*>(job_info);
	SCW_LDBG_ << "Start snapshot creation for name " << job_ptr->snapshot_name;
	if(!job_ptr->snapshot_name.size()) {
		err = -1;
		SCW_LERR_<< "The name of the snapshot is invalid";
	} else if((err = db_driver_ptr->snapshotCreateNewWithName(job_ptr->snapshot_name, job_ptr->job_work_code))) {
		SCW_LERR_<< "Error creating snapshot "<< job_ptr->snapshot_name <<" on database with error: " << err;
	} else {
		//all is gone ok..
		err = DataWorker::submitJobInfo(job_info);
	}
	return err;
}

int SnapshotCreationWorker::storeDatasetTypeInSnapsnot(const std::string& job_work_code,
													   const std::string& snapshot_name,
													   const std::string& unique_id,
													   const std::string& dataset_type) {
	int err = 0;
	void *data = NULL;
	uint32_t data_len = 0;
	
	//identify the dataaset
	std::string dataset_to_fetch = unique_id + dataset_type;
	
	SCW_LDBG_ << "Get live data for " << dataset_to_fetch << " in channel";
	if((err = cache_driver_ptr->getData((void*)dataset_to_fetch.c_str(), dataset_to_fetch.size(), &data, data_len))) {
		SCW_LERR_<< "Error retrieving live data for " << dataset_to_fetch << " with error: " << err;
	} else if(data) {
		SCW_LDBG_ << "Store data on snapshot for " << dataset_to_fetch;
		if((err = db_driver_ptr->snapshotAddElementToSnapshot(job_work_code,
															  snapshot_name,
															  unique_id,
															  dataset_to_fetch,
															  data,
															  data_len))) {
			SCW_LERR_<< "Error storign dataset type "<< dataset_type <<" for " << unique_id << " in snapshot " << snapshot_name << " with error: " << err;
		}
		
		free(data);
	}else {
		err = -1;
		SCW_LERR_<< "No data has been fetched for " << dataset_to_fetch;
	}
	return err;
}

void SnapshotCreationWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	int err = 0;
	
	//the set of unique id to snap
	std::vector<std::string> snapped_producer_keys;
	SnapshotCreationJob *job_ptr = reinterpret_cast<SnapshotCreationJob*>(job_info);
	
	if((err = db_driver_ptr->snapshotIncrementJobCounter(job_ptr->job_work_code,
														 job_ptr->snapshot_name,
														 true))) {
		SCW_LERR_<< "error incrementing the snapshot job counter for " << job_ptr->snapshot_name << " with error: " << err;
		//delete job memory
		free(job_info);
		
		return;
	}
	try {


		//recreate the array of producer key set
		if(job_ptr->concatenated_unique_id_memory_size) {
			std::string concatenated_keys((const char*)job_ptr->concatenated_unique_id_memory, job_ptr->concatenated_unique_id_memory_size);
			//split the concatenated string
			boost::split( snapped_producer_keys, concatenated_keys, is_any_of(","), token_compress_on );
			free(job_ptr->concatenated_unique_id_memory);
		}
		
		//get the unique id to snap
		if(snapped_producer_keys.size()) {
			SCW_LDBG_ << "make snapshot on the user producer'id set";
		} else {
			SCW_LDBG_ << "make snapshot on all producer key";
			mds_channel->getAllDeviceID(snapped_producer_keys);
		}
		
		//scann all id
		for (std::vector<std::string>::iterator it = snapped_producer_keys.begin();
			 it != snapped_producer_keys.end();
			 it++) {
			
			//snap output channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::OUTPUT_DATASE_PREFIX))) {
				
			}
			//snap input channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::INPUT_DATASE_PREFIX))) {
				
			}
			//snap custom channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::CUSTOM_DATASE_PREFIX))){
				
			}
			//snap system channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::SYSTEM_DATASE_PREFIX))) {
				
			}
		}
		
	}catch(...) {
		
	}
	
	if((err = db_driver_ptr->snapshotIncrementJobCounter(job_ptr->job_work_code,
														 job_ptr->snapshot_name,
														 false))) {
		SCW_LERR_<< "error decrementig the snapshot job counter for " << job_ptr->snapshot_name << " with error: " << err;
	}
	//delete job memory
	free(job_info);
}


void SnapshotCreationWorker::addServer(std::string server_description) {
	cache_driver_ptr->addServer(server_description);
}

void SnapshotCreationWorker::updateServerConfiguration() {
	cache_driver_ptr->updateConfig();
}