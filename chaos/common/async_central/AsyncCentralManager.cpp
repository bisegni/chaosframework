/*
 *	AsyncCentralManager.cpp
 *	!CHOAS
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

#include <chaos/common/global.h>
#include <chaos/common/async_central/AsyncCentralManager.h>
#include <chaos/common/async_central/TimerHandler.h>

#define ACM_LOG_HEAD "[AsyncCentralManager] - "

#define ACM_LAPP_ LAPP_ << ACM_LOG_HEAD
#define ACM_LDBG_ LDBG_ << ACM_LOG_HEAD
#define ACM_LERR_ LERR_ << ACM_LOG_HEAD


using namespace chaos::common::async_central;

void acm_timer(uv_timer_t *handle) {
	TimerHanlder *c_handler = reinterpret_cast<TimerHanlder*>(handle->data);
	c_handler->timeout();
}
void acm_thread(void *handle) {
	chaos::utility::delegate::Delegate *thread_delegate = reinterpret_cast<chaos::utility::delegate::Delegate*>(handle);
	(*thread_delegate)();
}
//----------------------------------------------------------------------------------------------------
//static initialization
bool AsyncCentralManager::looping = false;

AsyncCentralManager::AsyncCentralManager() {
	
}

AsyncCentralManager::~AsyncCentralManager() {
	
}


void AsyncCentralManager::_internalEventLoop(void *args) {
	
	uv_loop_t *uv_l_loc = reinterpret_cast<uv_loop_t*>(args);
	while(looping) {
		//call sync loop runner
		uv_run(uv_l_loc, UV_RUN_DEFAULT);
		//wait some little time
		usleep(500);
	}
	
}

// Initialize instance
void AsyncCentralManager::init(void *inti_data) throw(chaos::CException) {
	ACM_LAPP_ << "Allocating uv loop";
	uv_l = uv_loop_new();
	if(!uv_l) throw chaos::CException(-1, "Error allocating uv loop", __PRETTY_FUNCTION__);
	
	//start the interna central loop ticker
	ACM_LAPP_ << "Allocate loop thread";
	looping = true;
	if(uv_thread_create(&thread_loop_id, &AsyncCentralManager::_internalEventLoop, uv_l)) {
		//error creating thread
		throw CException(-1, "Error creating loop thread", __PRETTY_FUNCTION__);
	}
}

// Deinit the implementation
void AsyncCentralManager::deinit() throw(chaos::CException) {
	
	ACM_LAPP_ << "Stop event loop";
	looping = false;
	uv_stop(uv_l);
	
	ACM_LAPP_ << "Join loop thread";
	if(uv_thread_join(&thread_loop_id)) {
		ACM_LERR_ << "Error joining loop thread";
	}
}


int AsyncCentralManager::addTimer(TimerHanlder *timer_handler, uint64_t timeout,uint64_t repeat) {
	int err = 0;
	//init timer
	err = uv_timer_init(uv_l, &timer_handler->uv_t);
	if(err) return err;
	
	//start handler for timer
	err = uv_timer_start(&timer_handler->uv_t, acm_timer, timeout, repeat);
	if(err) return err;
	
	return err;
}

void AsyncCentralManager::removeTimer(TimerHanlder *timer_handler) {
	uv_timer_stop(&timer_handler->uv_t);
}

int AsyncCentralManager::addThread(chaos::utility::delegate::Delegate *thread_delegate, AcmThreadID *thread_id) {
	int err = 0;
	if((err = uv_thread_create(thread_id, &acm_thread, thread_delegate))) {
		return err;
	}
	return 0;
}

void AsyncCentralManager::joinThread(AcmThreadID *thread_id) {
	if(uv_thread_join(thread_id)) {
		ACM_LERR_ << "Error joining loop thread";
	}
}