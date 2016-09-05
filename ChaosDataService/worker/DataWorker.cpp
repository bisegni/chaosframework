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
#include <chaos/common/global.h>

using namespace chaos::common::utility;
using namespace chaos::data_service::worker;

#define DataWorker_LOG_HEAD "[DataWorker] - "

#define DCLAPP_ LAPP_ << DataWorker_LOG_HEAD
#define DCLDBG_ LDBG_ << DataWorker_LOG_HEAD << __FUNCTION__
#define DCLERR_ LERR_ << DataWorker_LOG_HEAD << __FUNCTION__ << " - " << __LINE__ << "-"

DataWorker::DataWorker():
work(false),
job_queue(1000),
max_element(1000){
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
    while(work) {
        thread_job = getNextOrWait(lock);
        if(thread_job) executeJob(thread_job, cookie);
        job_in_queue()--;
        DCLDBG_ << "job_in_queue:" << job_in_queue();
    }
}

void DataWorker::init(void *init_data) throw (chaos::CException) {
    job_in_queue() = 0;
    if(init_data) {
        std::memcpy(&settings, init_data, sizeof(DataWorkerSetting));
    } else {
        settings.job_thread_number = DEFAULT_JOB_THREAD;
    }
    
    thread_cookie = (void**)calloc(1, sizeof(void*)*settings.job_thread_number);
    DCLAPP_ << " Using " << settings.job_thread_number << " thread for consuming job";
}

void DataWorker::start() throw (chaos::CException) {
    work = true;
    for(int idx = 0; idx <settings.job_thread_number; idx++) {
        job_thread_group.create_thread(boost::bind(&DataWorker::consumeJob, this, thread_cookie[idx]));
    }
}

void DataWorker::stop() throw (chaos::CException) {
    work = false;
    job_condition.notify_all();
    job_thread_group.join_all();
}

void DataWorker::deinit() throw (chaos::CException) {
    WorkerJobPtr thread_job = NULL;
    //empty the job queue deleting the non executed job
    if(job_queue.empty() == false) {
        DCLAPP_ << "delete all remaining job";
        while(job_queue.pop(thread_job)) {
            DCLAPP_ << "delete worker job";
            delete(thread_job);
        }
    } else {
        DCLAPP_ << "job queue is empty";
    }
    if(thread_cookie) free(thread_cookie);
    
}

int DataWorker::submitJobInfo(WorkerJobPtr job_info) {
    LockableObjectWriteLock wl;
    job_in_queue.getWriteLock(wl);
    //check if we are out of max element in queue, in other case go out
    if(job_in_queue() > max_element) return -1000;
    
    //ad this element
    job_in_queue()++;
    
    if( job_queue.push(job_info) ) {
        job_condition.notify_one();
        return 0;
    }
    return -2;
}

void DataWorker::mantain() throw (chaos::CException) {
    
}
