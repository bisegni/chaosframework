/*
 *	MDSCronousManager.cpp
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

#include "MDSCronousManager.h"
#include "management/MDSHistoryAgeingManagement.h"

using namespace chaos::metadata_service::cron_job;

MDSCronousManager::MDSCronousManager():
abstract_persistance_driver(NULL){}

MDSCronousManager::~MDSCronousManager() {}

void MDSCronousManager::init(void *init_data) throw(chaos::CException) {
    CronousManager::init(init_data);
    std::string job_string;
    addJob(new MDSHistoryAgeingManagement(NULL),
           job_string,
           2000);
}

void MDSCronousManager::deinit() throw(chaos::CException) {
    CronousManager::deinit();
}

bool MDSCronousManager::addJob(MDSCronJob *new_job,
                               std::string& job_index,
                               uint64_t repeat_delay,
                               uint64_t offset) {
    if(new_job == NULL) return false;
    
    new_job->abstract_persistance_driver = abstract_persistance_driver;
    
    return CronousManager::addJob(new_job,
                                  job_index,
                                  repeat_delay,
                                  offset);
}
