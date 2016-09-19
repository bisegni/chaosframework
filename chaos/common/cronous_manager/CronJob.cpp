/*
 *	CronJob.cpp
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

#include <chaos/common/cronous_manager/CronJob.h>

using namespace chaos::common::cronous_manager;

#pragma mark Public Methos
CronJob::CronJob(chaos::common::data::CDataWrapper *job_parameter):
run_state(CronJobStateWaiting),
next_ts_start(0){
    //inizilize job map
    parserCDataWrapperForMapParameter(job_parameter);
}

CronJob::~CronJob() {}

#pragma mark Private Methos
void CronJob::parserCDataWrapperForMapParameter(chaos::common::data::CDataWrapper *param) {
    if(param == NULL) return;
    job_parameter.clear();
    
    ChaosStringVector keys;
    param->getAllKey(keys);
    
    //scann all keys and create map for input parameter
    for(ChaosStringVectorIterator it = keys.begin(),
        end = keys.end();
        it != end;
        it++) {
        //insert new parameter
        job_parameter.insert(MapKeyVariantPair(*it, param->getVariantValue(*it)));
    }
}
