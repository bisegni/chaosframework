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

#include "MDSCronusManager.h"
#include "../ChaosMetadataService.h"
#include "management/MDSHistoryAgeingManagement.h"

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::cron_job;

MDSCronusManager::MDSCronusManager():
CronusManager(ChaosMetadataService::getInstance()->setting.cron_job_scheduler_repeat_time*1000){}

MDSCronusManager::~MDSCronusManager() {}

void MDSCronusManager::init(void *init_data)  {
    CronusManager::init(init_data);
    std::string job_string;
    addJob(new MDSHistoryAgeingManagement(NULL),
           job_string,
           ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time*1000,
           ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time*1000);
}

void MDSCronusManager::deinit()  {
    CronusManager::deinit();
}

bool MDSCronusManager::addJob(MDSCronJob *new_job,
                               std::string& job_index,
                               uint64_t repeat_delay,
                               uint64_t offset) {
    if(new_job == NULL) return false;
    
    return CronusManager::addJob(new_job,
                                  job_index,
                                  repeat_delay,
                                  offset);
}
