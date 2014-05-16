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

using namespace chaos::data_service;

#define StageDataConsumer_LOG_HEAD "[StageDataConsumer] - "

#define SDCLAPP_ LAPP_ << StageDataConsumer_LOG_HEAD
#define SDCLDBG_ LDBG_ << StageDataConsumer_LOG_HEAD << __FUNCTION__ << " - "
#define SDCLERR_ LERR_ << StageDataConsumer_LOG_HEAD


StageDataConsumer::StageDataConsumer(vfs::VFSManager *_vfs_manager_instance, ChaosDataServiceSetting *_settings):
settings(_settings),
work_on_stage(false),
vfs_manager_instance(_vfs_manager_instance){
	
}

StageDataConsumer::~StageDataConsumer() {
	
}

void StageDataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!settings)  throw chaos::CException(-1, "No setting provided", __FUNCTION__);
	
	myself_delegate = common::utility::delegate::Delegate::from_method<StageDataConsumer, &StageDataConsumer::scanStage>(this);
	
	//add answer worker
	SDCLAPP_ << "Allocating stage data indexer";
	chaos::data_service::worker::IndexStageDataWorker *tmp_indexer_data_worker = NULL;
	
	//allocate the worker
	for(int idx = 0; idx < settings->indexer_worker_num; idx++) {
		tmp_indexer_data_worker = new chaos::data_service::worker::IndexStageDataWorker(vfs_manager_instance);
		tmp_indexer_data_worker->init(NULL);
		for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
			iter != settings->startup_chache_servers.end();
			iter++) {
				tmp_indexer_data_worker->init(init_data);
		}
		indexer_stage_worker_list.addSlot(tmp_indexer_data_worker);
	}

}

void StageDataConsumer::start() throw (chaos::CException) {
	
	//start the worker
	for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		SDCLAPP_ << "Stop stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->start();
	}
	
	SDCLAPP_ << "Start stage scanner thread";
	work_on_stage = true;
	for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		common::async_central::AcmThreadID threadID;
		chaos::common::async_central::AsyncCentralManager::getInstance()->addThread(&myself_delegate, &threadID);
		thread_ids.push_back(threadID);
	}
}

void StageDataConsumer::stop() throw (chaos::CException) {
	SDCLAPP_ << "Stop stage scanner thread";
	work_on_stage = false;
	for(int idx = 0; idx < thread_ids.size(); idx++) {
		chaos::common::async_central::AsyncCentralManager::getInstance()->joinThread(&thread_ids[idx]);
	}
	thread_ids.clear();
	
	for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		SDCLAPP_ << "Stop stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->stop();
	}
}

void StageDataConsumer::deinit() throw (chaos::CException) {
	for(int idx = 0; idx < indexer_stage_worker_list.getNumberOfSlot(); idx++) {
		SDCLAPP_ << "Deallocating stage data indexer of idx " << idx;
		chaos::data_service::worker::DataWorker *worker = indexer_stage_worker_list.accessSlotByIndex(idx);
		worker->deinit();
		delete(worker);
	}
}

void StageDataConsumer::scanStage() {
	SDCLAPP_ << "Enter stage scanner thread";
	//get stage reading file
	
	while(work_on_stage) {
		
	}
	SDCLAPP_ << "Leaving stage scanner thread";
}