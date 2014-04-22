//
//  DataWorker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <cstring>
#include "DataWorker.h"
#include <boost/thread/lock_options.hpp>
using namespace chaos::data_service::worker;

#define DataWorker_LOG_HEAD "[DataWorker] - "

#define DCLAPP_ LAPP_ << DataWorker_LOG_HEAD
#define DCLDBG_ LDBG_ << DataWorker_LOG_HEAD
#define DCLERR_ LERR_ << DataWorker_LOG_HEAD

DataWorker::DataWorker():work(false), job_queue(1000),job_in_queue(0) {
	std::memset(&settings, 0, sizeof(DataWorkerSetting));
}

DataWorker::~DataWorker() {
	
}

WorkerJobPtr DataWorker::getNextOrWait(boost::unique_lock<boost::mutex>& lock) {
	WorkerJobPtr new_job = NULL;
	while(!job_queue.pop(new_job) && work) {
		job_condition.wait(lock);
	}
	return new_job;
}

void DataWorker::consumeJob(void *cookie) {
	WorkerJobPtr thread_job = NULL;
	boost::unique_lock<boost::mutex> lock(mutex_job);
	DCLAPP_<< "Entering in working cicle";
	while(work) {
		thread_job = getNextOrWait(lock);
		if(thread_job) executeJob(thread_job, cookie);
		job_in_queue--;
	}
	DCLAPP_<< "Working cicle temrinated";
}

void DataWorker::init(void *init_data) throw (chaos::CException) {
	DCLAPP_<< "Get the settings";
	if(init_data) {
		std::memcpy(&settings, init_data, sizeof(DataWorkerSetting));
	} else {
		settings.job_thread_number = DEFAULT_JOB_THREAD;
	}
	
	thread_cookie = (void**)malloc(sizeof(void*)*settings.job_thread_number);
	DCLAPP_ << " Using " << settings.job_thread_number << " thread for consuming job";
}

void DataWorker::start() throw (chaos::CException) {
	DCLAPP_ << " Starting " << settings.job_thread_number << " thread for consuming job";
	work = true;
	for(int idx = 0; idx <settings.job_thread_number; idx++) {
		job_thread_group.create_thread(boost::bind(&DataWorker::consumeJob, this, thread_cookie[idx]));
	}
}

void DataWorker::stop() throw (chaos::CException) {
	DCLAPP_ << " Stopping " << settings.job_thread_number << " thread for consuming job";
	work = false;
	//for(int idx = 0; idx <settings.job_thread_number; idx++) {
	job_condition.notify_all();
	//}
	job_thread_group.join_all();

}

void DataWorker::deinit() throw (chaos::CException) {
	WorkerJobPtr thread_job = NULL;
	//empty the job queue deleting the non executed job
	DCLAPP_ << "delete all remaining job";
	while(job_queue.pop(thread_job)) {
		DCLAPP_ << "delete worker job";
		delete(thread_job);
	}
	
	if(thread_cookie) free(thread_cookie);
	DCLAPP_ << "job queue is empty";
}

bool DataWorker::submitJobInfo(WorkerJobPtr job_info) {
	if(job_in_queue++ % 1000) {
		DCLDBG_ << "Job in queue " << job_in_queue;
	}
	if( job_queue.push(job_info) ) {
		job_condition.notify_one();
		return true;
	}
	return false;
}