/*
 *	cronous_manager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/09/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/cronous_manager/CronousManager.h>

using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::cronous_manager;

#define CHECK_TIME 10000
#define CHECK_THREAD_MAX_ELEMENT 5
#define MAX_NUMBER_OF_CONCURRENT_EXECUTION 10

#pragma mark Public Methods
void CronousManager::init(void *init_data) throw (chaos::CException) {
    //register as timer
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 CHECK_TIME,
                                                 CHECK_TIME);
}

void CronousManager::deinit() throw (chaos::CException) {
    //register as timer
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void CronousManager::addJob(CronJob *new_job,
                            uint64_t repeat_delay,
                            uint64_t offset) {
    if(new_job == NULL) return;
    LockableObjectWriteLock wl;
    
    vector_job_instance.getWriteLock(wl);
    new_job->next_ts_start = (TimingUtil::getTimeStamp()+offset);
    vector_job_instance().push_back(JobInstanceShrdPtr(new_job));
}

#pragma mark Protected Methods
void CronousManager::timout() {
    //scan job to start
    uint64_t current_ts = TimingUtil::getTimeStamp();
    //scann instance to determinate wich one need to be started
    if(vector_job_in_execution.size()<MAX_NUMBER_OF_CONCURRENT_EXECUTION) {
        //lock the vecto rof instance
        LockableObjectWriteLock wl_on_instance;
        vector_job_instance.getWriteLock(wl_on_instance);
        
        //scans instance to find wich one need to be started
        for(VectorJobInstanceIterator it = vector_job_instance().begin(),
            end = vector_job_instance().end();
            it != end;
            it++) {
            if((*it)->run_state == CronJobStateWaiting &&
               current_ts < (*it)->next_ts_start) {
                (*it)->run_state = CronJobStateRunning;
                //start the execution of the job into a thread
                vector_job_in_execution.push_back(ThreadJobShrdPtr(new boost::thread(boost::bind(&CronJob::execute,
                                                                                                 (*it).get(),
                                                                                                 (*it)->job_parameter))));
                //check if we have reached the maximum number fo concurrent jobs
                if(vector_job_in_execution.size()>=MAX_NUMBER_OF_CONCURRENT_EXECUTION){break;}
            }
        }
    }
    
    //check started job to determinate which has finisched
    unsigned int element_count = 0;
    VectorJobThreadIterator it = vector_job_in_execution.begin();
    VectorJobThreadIterator end = vector_job_in_execution.end();
    while(it != end) {
        if((*it)->joinable()){
            if((*it)->try_join_for(boost::chrono::milliseconds(10))){
                //remove finisched thread
                it = vector_job_in_execution.erase(it);
            } else {
                it++;
            }
        }
        //stop in case we have processed the maximum number of thread
        if(element_count++>=CHECK_THREAD_MAX_ELEMENT){break;}
    }
}

#pragma mark Private Methods
CronousManager::CronousManager() {
    
}

CronousManager::~CronousManager() {
    
}

void CronousManager::clearCompletedJob() {
    
}

void CronousManager::scanForJobToLaunch() {
    
}

