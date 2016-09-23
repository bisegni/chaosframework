/*
 *	main.cpp
 *	!CHAOS
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

#include <iostream>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include "ChaosMetadataService.h"

using namespace chaos::metadata_service;

int main(int argc, char * argv[]) {
	try {
		//cache parameter
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_BATCH_SANDBOX_SIZE,
                                                                                                    "Specify number of the sanbox to use into the batch subsytem",
                                                                                                    &ChaosMetadataService::getInstance()->setting.batch_sandbox_size);
        
		ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_PERSITENCE_IMPL,
																										"Specify the implementation of the persistence layer",
																										&ChaosMetadataService::getInstance()->setting.persistence_implementation);
		
		ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_PERSITENCE_SERVER_ADDR_LIST,
																													 "Specify servers where the persistence layer needs to use",
																													 &ChaosMetadataService::getInstance()->setting.persistence_server_list);
		
		ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_PERSITENCE_KV_PARAMTER,
																													 "The key value parameter for storage implementation driver (ex k:v-k1:v1)");
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CRON_JOB_CHECK,
                                                                                                         "Specify the check time (in seconds ) for the cron job scheduler",
                                                                                                         3600,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cron_job_scheduler_repeat_time);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CRON_JOB_AGEING_MANAGEMENT,
                                                                                                         "Specify the check time (in seconds ) for ageing management (default is one day of delay)",
                                                                                                         86400,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time);
		ChaosMetadataService::getInstance()->init(argc, argv);
		
		ChaosMetadataService::getInstance()->start();
	} catch (chaos::CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
	return 0;
}

