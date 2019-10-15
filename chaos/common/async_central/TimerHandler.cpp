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

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/async_central/TimerHandler.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define INFO    INFO_LOG(TimerHandler)
#define DBG     DBG_LOG(TimerHandler)
#define ERR     ERR_LOG(TimerHandler)

using namespace chaos::common::utility;
using namespace chaos::common::async_central;

TimerHandler::TimerHandler():
terminated(false),
timer(NULL),
delay(0){}

TimerHandler::~TimerHandler() {
    delete(timer);
}

void TimerHandler::timerTimeout(const boost::system::error_code& error) {
   if(!error && !terminated){
        uint64_t start_ts = TimingUtil::getTimeStamp();
        //call timer handler
        timeout();
        
        uint64_t spent_time = TimingUtil::getTimeStamp()-start_ts;
        
        //wait for next call with the delat correct
        wait(delay-spent_time);
   } else if (error ==  boost::asio::error::operation_aborted) {
       ERR << "Timer thread: " << boost::this_thread::get_id() << ", handle_timeout error " << error.message();
   }
    if(terminated){wait_sem.unlock();}
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
    terminated = true;
    std::size_t remain = 0;
    try{
        remain = timer->cancel();
    } catch(boost::system::system_error& ex) {}
    if(remain)wait_sem.wait();
    delete(timer);
    timer = NULL;
}
