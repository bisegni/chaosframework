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

#include "MDSCronousManager.h"
#include "../ChaosMetadataService.h"
#include "management/MDSHistoryAgeingManagement.h"

using namespace chaos::metadata_service::cron_job;

MDSCronousManager::MDSCronousManager():
CronousManager(ChaosMetadataService::getInstance()->setting.cron_job_scheduler_repeat_time*1000),
abstract_persistance_driver(NULL){}

MDSCronousManager::~MDSCronousManager() {}

void MDSCronousManager::init(void *init_data) throw(chaos::CException) {
    CronousManager::init(init_data);
    std::string job_string;
    addJob(new MDSHistoryAgeingManagement(NULL),
           job_string,
           ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time*1000,
           ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time*1000);
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
