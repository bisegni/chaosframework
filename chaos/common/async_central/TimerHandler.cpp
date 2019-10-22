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

#include <cmath>

#define INFO    INFO_LOG(TimerHandler)
#define DBG     DBG_LOG(TimerHandler)
#define ERR     ERR_LOG(TimerHandler)

using namespace chaos::common::utility;
using namespace chaos::common::async_central;

TimerHandler::TimerHandler():
stoppped(false),
need_signal(false),
cicle_test(false),
delay(0){}

TimerHandler::~TimerHandler() {}

void TimerHandler::reset() {
    need_signal = false;
    stoppped = false;
    cicle_test = false;
    timer.reset();
    delay = 0;
}

void TimerHandler::timerTimeout(const boost::system::error_code& error) {
    if(error) return;
    cicle_test = false;
    boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
    if(!stoppped){
        int64_t start_ts = TimingUtil::getTimeStamp();
        //call timer handler
        timeout();
        int64_t spent_time = TimingUtil::getTimeStamp()-start_ts;
        //wait for next call with the delat correct
        if(std::abs(spent_time) > delay) {
            //quantize the slot
            lldiv_t divresult = std::lldiv(std::abs(spent_time),delay);
            wait(divresult.rem);
        } else {
            wait(delay-spent_time);
        }
    }
    if(stoppped){
        cicle_test = false;
    }
    if(need_signal){wait_answer_condition.notify_one();}
}

void TimerHandler::wait(int64_t _delay) {
    if(timer.get() == NULL) return;
    //repeat rate
    cicle_test = true;
    timer->expires_from_now(boost::posix_time::milliseconds(_delay));
    timer->async_wait(boost::bind(&TimerHandler::timerTimeout,
                                  this,
                                  boost::asio::placeholders::error));
}

void TimerHandler::removeTimer() {
    boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
    stoppped = true;
    std::size_t remain = 0;
    try{
        if(timer.get()) {
            remain = timer->cancel();
        }
    } catch(boost::system::system_error& ex) {}
    if((remain==0) && cicle_test) {
        need_signal = true;
        //we are in acse of a timer task can't be stopped
        while(cicle_test){wait_answer_condition.wait(lock);};
    }
    
}
