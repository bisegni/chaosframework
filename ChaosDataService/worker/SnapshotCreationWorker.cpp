/*
 *	SnapshotCreationWorker.cpp
 *	!CHOAS
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

#include "SnapshotCreationWorker.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>

using namespace chaos::common::network;
using namespace chaos::data_service::worker;

#define SnapshotCreationWorker_LOG_HEAD "[SnapshotCreationWorker] - "
#define SCW_LAPP_ LAPP_ << SnapshotCreationWorker_LOG_HEAD
#define SCW_LDBG_ LDBG_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - "
#define SCW_LERR_ LERR_ << SnapshotCreationWorker_LOG_HEAD << __FUNCTION__ << " - " << __LINE__ << "-"


//------------------------------------------------------------------------------------------------------------------------

SnapshotCreationWorker::SnapshotCreationWorker(const std::string& _cache_impl_name,
											   const std::string& _db_impl_name,
											   NetworkBroker	*_network_broker):
cache_impl_name(_cache_impl_name),
db_impl_name(_db_impl_name),
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
	cache_driver_ptr = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name);
	
	SCW_LAPP_ << "allocating cache driver";
	db_driver_ptr = chaos::ObjectFactoryRegister<db_system::DBDriver>::getInstance()->getNewInstanceByName(db_impl_name);
}

void SnapshotCreationWorker::deinit() throw (chaos::CException) {
	SCW_LAPP_ << "deallocating cache driver";
	if(cache_driver_ptr) delete(cache_driver_ptr);
	SCW_LAPP_ << "deallocating db driver";
	if(db_driver_ptr) delete(db_driver_ptr);
	
	if(mds_channel && network_broker) {
		network_broker->disposeMessageChannel(mds_channel);
	}
	
	DataWorker::deinit();
}

void SnapshotCreationWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	SnapshotCreationJob *job_ptr = reinterpret_cast<SnapshotCreationJob*>(job_info);
	//check what kind of push we have
	//read lock on mantainance mutex
	SCW_LDBG_ << "Start snapshot creation for name" << job_ptr->snapshot_name;
	
	if(job_ptr->produceter_unique_id_set.size()) {
		SCW_LDBG_ << "snapshot the user set";
	} else {
		SCW_LDBG_ << "Retrieve all producer key to snapshot all";
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