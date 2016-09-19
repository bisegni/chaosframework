/*
 *	AsyncCentralManager.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/async_central/AsyncCentralManager.h>
#include <chaos/common/async_central/TimerHandler.h>

#define ACM_LOG_HEAD "[AsyncCentralManager] - "

#define ACM_LAPP_ LAPP_ << ACM_LOG_HEAD
#define ACM_LDBG_ LDBG_ << ACM_LOG_HEAD
#define ACM_LERR_ LERR_ << ACM_LOG_HEAD

using namespace boost::asio;
using namespace chaos::common::async_central;

//----------------------------------------------------------------------------------------------------


AsyncCentralManager::AsyncCentralManager():
asio_service(),
asio_default_work(asio_service) {
}

AsyncCentralManager::~AsyncCentralManager() {
}

// Initialize instance
void AsyncCentralManager::init(void *init_data) throw(chaos::CException) {
    ACM_LAPP_ << "Allocating event loop";
    asio_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &asio_service));
}

// Deinit the implementation
void AsyncCentralManager::deinit() throw(chaos::CException) {
    ACM_LAPP_ << "Stop event loop";
    asio_service.stop();
    asio_thread_group.join_all();
}

int AsyncCentralManager::addTimer(TimerHandler *timer_handler,
                                  uint64_t timeout,
                                  uint64_t repeat) {
    int err = 0;
    try{
        boost::unique_lock<boost::mutex> l(mutex);
        //check if already installed
        if(timer_handler->timer) return 0;
        
        if((timer_handler->timer = new  deadline_timer(asio_service)) == NULL) {
            err = -1;
        } else {
            timer_handler->delay = repeat;
            timer_handler->wait(timeout);
        }
    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::system::system_error> >& ex){
        err = -2;
    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::lock_error> > & ex) {
        err = -3;
    }
    return err;
}


int AsyncCentralManager::removeTimer(TimerHandler *timer_handler) {
    int err = 0;
    try{
        boost::unique_lock<boost::mutex> l(mutex);
        timer_handler->removeTimer();
    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::system::system_error> >& ex){
        err = -2;
    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::lock_error> > & ex) {
        err = -3;
    }
    return err;
}
