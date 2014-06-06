/*
 *	StageDataConsumer.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include "StageDataConsumer.h"
#include "worker/IndexStageDataWorker.h"

#include <unistd.h>
#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::data_service;

#define StageDataConsumer_LOG_HEAD "[StageDataConsumer] - "

#define StageDataConsumerLAPP_ LAPP_ << StageDataConsumer_LOG_HEAD
#define StageDataConsumerLDBG_ LDBG_ << StageDataConsumer_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataConsumerLERR_ LERR_ << StageDataConsumer_LOG_HEAD

#define DISPOSE_SCANNER_INFO(s) \
delete (s->scanner); \
delete s; \
s=NULL;


#define LOCK_SCANNER_INFO(s) \
boost::unique_lock<boost::mutex> l(s->mutex_on_scan);

StageDataConsumer::StageDataConsumer(vfs::VFSManager *_vfs_manager_instance, ChaosDataServiceSetting *_settings):
settings(_settings),
work_on_stage(false),
vfs_manager_instance(_vfs_manager_instance),
global_scanner_num(0),
queue_scanners(1) {
	
}

StageDataConsumer::~StageDataConsumer() {
	
}

void StageDataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!settings)  throw chaos::CException(-1, "No setting provided", __FUNCTION__);
	
	//add answer worker
	StageDataConsumerLAPP_ << "Allocating stage data indexer";
	chaos::data_service::worker::IndexStageDataWorker *tmp_indexer_data_worker = NULL;
	
	//allocate the worker
	/*for(int idx = 0; idx < settings->indexer_worker_num; idx++) {
		tmp_indexer_data_worker = new chaos::data_service::worker::IndexStageDataWorker(vfs_manager_instance);
		tmp_indexer_data_worker->init(NULL);
		for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
			iter != settings->startup_chache_servers.end();
			iter++) {
			tmp_indexer_data_worker->init(init_data);
		}
		indexer_stage_worker_list.addSlot(tmp_indexer_data_worker);
	}*/
	
}

void StageDataConsumer::start() throw (chaos::CException) {
	
	//start the worker
	/*for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		StageDataConsumerLAPP_ << "Stop stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->start();
	}*/
	
	StageDataConsumerLAPP_ << "Start find path timer";
	//scan path every 60 seconds
	chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, 0, 60000);
	
	StageDataConsumerLAPP_ << "Start stage scanner thread";
	work_on_stage = true;
	for(int idx = 0; idx < settings->indexer_worker_num; idx++) {
		thread_scanners.add_thread(new boost::thread(boost::bind(&StageDataConsumer::scanStage, this)));
	}
}

void StageDataConsumer::stop() throw (chaos::CException) {
	StageDataConsumerLAPP_ << "Stop find path timer";
	chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);
	
	StageDataConsumerLAPP_ << "Stop stage scanner thread";
	work_on_stage = false;
	thread_scanners.join_all();
	
	//clear the porcessi queue whitout delete the element
	StageScannerInfo *scanner_info = NULL;
	while(!queue_scanners.empty()) {
		queue_scanners.pop(scanner_info);
		//delete the element
		DISPOSE_SCANNER_INFO(scanner_info)
	}
	// now clear the superclass hashtable that will erase the element
	
	/*for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		StageDataConsumerLAPP_ << "Stop stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->stop();
	}*/
}

void StageDataConsumer::deinit() throw (chaos::CException) {
	/*for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		StageDataConsumerLAPP_ << "Deallocating stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->deinit();
		delete(worker);
	}*/
}

void StageDataConsumer::timeout() {
	int err = 0;
	std::vector<std::string> current_stage_file;
	
	StageScannerInfo *scanner_info = NULL;
	vfs::VFSStageReadableFile *readable_stage_file = NULL;
	
	if(vfs_manager_instance->getAllStageFileVFSPath(current_stage_file)) return;
	
	//cicle all found vfs file path to search wich are new
	for (std::vector<std::string>::iterator it = current_stage_file.begin();
		 it != current_stage_file.end();
		 it++) {
		//check if we alread have this path
		if(std::find(vector_working_path.begin(), vector_working_path.end(), *it) == vector_working_path.end()) {
			//get new vfs stage readable file
			if((err = vfs_manager_instance->getReadableStageFile(*it, &readable_stage_file))) {
				//error gettin file
				StageDataConsumerLDBG_ << "Error getting vfs stage readable file for vfs path: " << *it;
				continue;
			}
			
			//scanner not present, so we need to add it
			scanner_info = new StageScannerInfo();
			scanner_info->index = ++global_scanner_num;
			scanner_info->scanner = new index_system::StageDataVFileScanner(readable_stage_file);
			
			//add new scanner infor to the processing queue to be scheduled and in vector to keep track of it
			vector_working_path.push_back(*it);
			queue_scanners.push(scanner_info);
		}
	}
}

StageScannerInfo *StageDataConsumer::getNextAvailableScanner() {
	StageScannerInfo *result = NULL;
	StageScannerInfo *scanner_info = NULL;
	uint32_t n_element = global_scanner_num;
	while (n_element-->0) {
		queue_scanners.pop(scanner_info);
		
		LOCK_SCANNER_INFO(scanner_info)
		
		if(scanner_info->in_use) continue;
		
		scanner_info->in_use = true;
		result = scanner_info;
		break;
	}
	return result;
}

void StageDataConsumer::rescheduleScannerInfo(StageScannerInfo *scanner_info) {
	LOCK_SCANNER_INFO(scanner_info)
	scanner_info->in_use = false;
	queue_scanners.push(scanner_info);
}

void StageDataConsumer::scanStage() {
	StageDataConsumerLAPP_ << "Enter stage scanner thread";
	StageScannerInfo *scanner_info = NULL;
	//
	while(work_on_stage) {
		if((scanner_info = getNextAvailableScanner())) {
			StageDataConsumerLAPP_ << "Schedule scanner "<< scanner_info->index << " of" << global_scanner_num << " for file ->"<< scanner_info->scanner->getScannedVFSPath();
			// scan a compelte block
			scanner_info->scanner->scan();
			//give some time to kernel
			
			StageDataConsumerLAPP_ << "End scanner for file ->"<< scanner_info->scanner->getScannedVFSPath();
			
			rescheduleScannerInfo(scanner_info);
		}
		
		boost::this_thread::sleep(boost::posix_time::milliseconds(250));
	}
	StageDataConsumerLAPP_ << "Leaving stage scanner thread";
}