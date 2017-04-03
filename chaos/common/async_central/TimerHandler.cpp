/*
 *	TimerHandler.cpp
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/async_central/TimerHandler.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::async_central;

TimerHandler::TimerHandler():
timer(NULL){}

TimerHandler::~TimerHandler() {}

void TimerHandler::timerTimeout(const boost::system::error_code& error) {
    if (error ==  boost::asio::error::operation_aborted) {
        return;
    } else {
        uint64_t start_ts = TimingUtil::getTimeStamp();
        //call timer handler
        timeout();
        
        uint64_t spent_time = TimingUtil::getTimeStamp()-start_ts;
        
        //wait for next call with the delat correct
        wait(delay-spent_time);
    }
    if(wait_sem.isInWait()){wait_sem.unlock();}
}

void TimerHandler::wait(uint64_t _delay) {
    if(timer == NULL) return;
    //repeat rate
    timer->expires_from_now(boost::posix_time::milliseconds(_delay));
    timer->async_wait(boost::bind(&TimerHandler::timerTimeout,
                                  this,
                                  boost::asio::placeholders::error));
}

void TimerHandler::removeTimer() {
    if(timer == NULL) return;
    std::size_t remain = 0;
    try{
        remain = timer->cancel();
        if(remain == 0) {
            //waith the signal
            wait_sem.wait();
        }
    } catch(boost::system::system_error& ex) {}

    delete(timer);
    timer = NULL;
}
