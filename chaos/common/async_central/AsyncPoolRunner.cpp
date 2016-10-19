/*
 *	AsyncPoolRunner.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/10/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/async_central/AsyncPoolRunner.h>

using namespace chaos;
using namespace chaos::common::async_central;

#define APL_INFO_LOG    INFO_LOG(AsyncPoolRunner)
#define APL_ERR_LOG     ERR_LOG(AsyncPoolRunner)
#define APL_DBG_LOG     DBG_LOG(AsyncPoolRunner)

#pragma AsyncRunnable Public Methods
AsyncRunnable::AsyncRunnable(const std::string& _job_desc):
job_desc(_job_desc){}

AsyncRunnable::~AsyncRunnable() {}

#pragma Public Methods
AsyncPoolRunner::AsyncPoolRunner(unsigned int _thread_number):
thread_number(_thread_number){}

AsyncPoolRunner::~AsyncPoolRunner() {}

void AsyncPoolRunner::init(void *init_data) throw(chaos::CException) {
    PriorityQueueRunnable::init(thread_number);
}

void AsyncPoolRunner::deinit() throw(chaos::CException) {
    PriorityQueueRunnable::deinit(true);
}

void AsyncPoolRunner::submit(AsyncRunnable *runnable) {
    PriorityQueueRunnable::push(runnable);
}

#pragma Protected Methods
void AsyncPoolRunner::processBufferElement(AsyncRunnable *next_job,
                                           ElementManagingPolicy& element_policy) throw(CException) {
    try {
       next_job->run();
    } catch (CException& ex) {
        APL_ERR_LOG << ex.what();
    } catch (std::exception& ex) {
        APL_ERR_LOG << ex.what();
    } catch (...) {
        APL_ERR_LOG << "unknown exception";
    }

}
