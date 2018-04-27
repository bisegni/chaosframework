/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include "DataWorker.h"
#include "../ChaosMetadataService.h"

#include <boost/thread/lock_options.hpp>
#include <chaos/common/global.h>

#include <cstring>

using namespace chaos::common::utility;
using namespace chaos::metadata_service;
using namespace chaos::data_service::worker;

#define DataWorker_LOG_HEAD "[DataWorker] - "

#define DCLAPP_ INFO_LOG(DataWorker)
#define DCLDBG_ DBG_LOG(DataWorker)
#define DCLERR_ ERR_LOG(DataWorker)

DataWorker::DataWorker():
work(false),
job_queue(1000),
max_element(1000){}

DataWorker::~DataWorker() {}

WorkerJobPtr DataWorker::getNextOrWait(boost::unique_lock<boost::mutex>& lock) {
    WorkerJobPtr new_job = NULL;
    while(!job_queue.pop(new_job) && work) {
        consume_job_condition.wait(lock);
    }
    return new_job;
}

void DataWorker::consumeJob(void *cookie) {
    WorkerJobPtr thread_job = NULL;
    boost::unique_lock<boost::mutex> lock(mutex_job);
    while(work) {
        //wait for next thread
        thread_job = getNextOrWait(lock);
        //decrease job in queue
        job_in_queue--;
        //unlock for next data
        push_condition.notify_one();
        //execute job
        if(thread_job) {
            executeJob(thread_job, cookie);
        }
    }
}

void DataWorker::init(void *init_data) throw (chaos::CException) {
    job_in_queue = 0;
    thread_cookie = (void**)calloc(1, sizeof(void*)*ChaosMetadataService::getInstance()->setting.worker_setting.thread_number);
    DCLAPP_ << " Using " << ChaosMetadataService::getInstance()->setting.worker_setting.thread_number << " thread for consuming job";
}

void DataWorker::start() throw (chaos::CException) {
    work = true;
    for(int idx = 0; idx < ChaosMetadataService::getInstance()->setting.worker_setting.thread_number; idx++) {
        job_thread_group.create_thread(boost::bind(&DataWorker::consumeJob, this, thread_cookie[idx]));
    }
}

void DataWorker::stop() throw (chaos::CException) {
    work = false;
    consume_job_condition.notify_all();
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

void DataWorker::setMaxElement(uint64_t new_max_element) {
    job_in_queue = new_max_element;
}

int DataWorker::submitJobInfo(WorkerJobPtr job_info, int64_t milliseconds_to_wait) {
    boost::unique_lock<boost::mutex> lock(mutex_submit);
    //check if we are out of max element in queue, in other case go out
    if(job_in_queue >= max_element) {
        DCLDBG_ << "Fifo Full queue :"<<job_in_queue<<", waiting..";

        if(push_condition.wait_for(lock, boost::chrono::milliseconds(milliseconds_to_wait)) == boost::cv_status::timeout) {
            DCLERR_ << "Datapack has gone in timeout waiting for queue free more space";
            return -1;
        }
    }
    //ad this element
    job_in_queue++;
    
    if( job_queue.push(job_info) ) {
        consume_job_condition.notify_one();
        return 0;
    }
    return -2;
}
