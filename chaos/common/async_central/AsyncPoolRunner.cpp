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
