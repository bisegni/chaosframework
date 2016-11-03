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
#include "../DriverPoolManager.h"
#include "SnapshotCreationWorker.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::network;

using namespace chaos::data_service;
using namespace chaos::data_service::worker;

#define SnapshotCreationWorker_LOG_HEAD "[SnapshotCreationWorker] - "
#define SCW_LAPP_ LAPP_ << SnapshotCreationWorker_LOG_HEAD
#define SCW_LDBG_ LDBG_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - "
#define SCW_LERR_ LERR_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - " << __LINE__ << "-"


//------------------------------------------------------------------------------------------------------------------------

SnapshotCreationWorker::SnapshotCreationWorker(db_system::DBDriver	*_db_driver_ptr,
											   NetworkBroker	*_network_broker):
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
}

void SnapshotCreationWorker::deinit() throw (chaos::CException) {
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
    
    CachePoolSlot *cache_slot = DriverPoolManager::getInstance()->getCacheDriverInstance();
    try{
        //get data
        if((err = cache_slot->resource_pooled->getData((void*)dataset_to_fetch.c_str(), dataset_to_fetch.size(), &data, data_len))) {
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
    } catch(...) {
        
    }
    DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
	return err;
}

#define FREE_JOB(x)\
if(x->concatenated_unique_id_memory_size) {free(x->concatenated_unique_id_memory);}\
free(x);

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
		FREE_JOB(job_ptr)
		return;
	}
	try {


		//recreate the array of producer key set
		if(job_ptr->concatenated_unique_id_memory_size) {
			std::string concatenated_keys((const char*)job_ptr->concatenated_unique_id_memory, job_ptr->concatenated_unique_id_memory_size);
			//split the concatenated string
			boost::split( snapped_producer_keys, concatenated_keys, is_any_of(","), token_compress_on );
		}
		
		//get the unique id to snap
		if(snapped_producer_keys.size()) {
			SCW_LDBG_ << "make snapshot on the user producer id set";
		} else {
			SCW_LERR_ << "Snapshoot need to have target node id";
            FREE_JOB(job_ptr)
            return;
		}
		
		//scann all id
		for (std::vector<std::string>::iterator it = snapped_producer_keys.begin();
			 it != snapped_producer_keys.end();
			 it++) {
			
			//snap output channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::OUTPUT_DATASET_POSTFIX))) {
				
			}
			//snap input channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::INPUT_DATASET_POSTFIX))) {
				
			}
			//snap custom channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::CUSTOM_DATASET_POSTFIX))){
				
			}
			//snap system channel
			if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code, job_ptr->snapshot_name, *it, DataPackPrefixID::SYSTEM_DATASET_POSTFIX))) {
				
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
	FREE_JOB(job_ptr)
}
