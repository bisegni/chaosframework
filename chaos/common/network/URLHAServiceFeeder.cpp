/*
 *	URLHAServiceFeeder.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 08/02/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/network/URLHAServiceFeeder.h>

#define RETRY_TIME 1000

#define URLHASF_INFO    INFO_LOG(URLHAServiceFeeder) << "["<<getName()<<"] - "
#define URLHASF_DBG     DBG_LOG(URLHAServiceFeeder) << "["<<getName()<<"] - "
#define URLHASF_ERR     ERR_LOG(URLHAServiceFeeder) << "["<<getName()<<"] - "

using namespace chaos::common::network;
using namespace chaos::common::utility;

URLHAServiceFeeder::URLHAServiceFeeder(std::string alias,
                                       URLServiceFeederHandler *_handler,
                                       URLHAServiceCheckerFeederHandler *_service_checker_handler):
URLServiceFeeder(alias,
                 _handler),
service_checker_handler(_service_checker_handler) {
    setRetryTime(1000, 10000);
}

URLHAServiceFeeder::~URLHAServiceFeeder() {}

void URLHAServiceFeeder::setRetryTime(const uint32_t _min_retry_time,
                                      const uint32_t _max_retry_time) {
    min_retry_time = _min_retry_time;
    max_retry_time = _max_retry_time;
}

uint32_t URLHAServiceFeeder::addURL(const URL& new_url,
                                    uint32_t priority) {
    return URLServiceFeeder::addURL(new_url,
                                    priority);
}

void URLHAServiceFeeder::removeURL(uint32_t idx,
                                   bool dispose_service) {
    URLServiceFeeder::removeURL(idx,
                                dispose_service);
}


void URLHAServiceFeeder::clear(bool dispose_service) {
    URLServiceFeeder::clear(dispose_service);
}

void* URLHAServiceFeeder::getService() {
    //!befor return renable respauned services
    boost::unique_lock<boost::mutex> wr(mutex_queue);
    while (respawned_queue.size()) {
        URLServiceFeeder::setURLOnline(respawned_queue.front());
        respawned_queue.pop();
    }
    wr.unlock();
    return URLServiceFeeder::getService();
}

//! set url has offline
void URLHAServiceFeeder::setURLAsOffline(const std::string& remote_address) {
    ServiceRetryInformation sri(getIndexFromURL(remote_address),
                                remote_address);
    sri.retry_timeout = TimingUtil::getTimeStamp()+min_retry_time;
    
    boost::unique_lock<boost::mutex> wr(mutex_queue);
    retry_queue.push(sri);
}

//! set url has offline
void URLHAServiceFeeder::setIndexAsOffline(const uint32_t remote_index) {
    ServiceRetryInformation sri(remote_index,
                                getURLForIndex(remote_index));
    sri.retry_timeout = TimingUtil::getTimeStamp()+min_retry_time;
    boost::unique_lock<boost::mutex> wr(mutex_queue);
    retry_queue.push(sri);
}

void URLHAServiceFeeder::checkForAliveService() {
    uint64_t current_ts = TimingUtil::getTimeStamp();
    
    while (retry_queue.size()) {
        ServiceRetryInformation sri = retry_queue.front();retry_queue.pop();
        sri.retry_times++;
        if(current_ts>= sri.retry_timeout) {
            if(service_checker_handler->serviceOnlineCheck(URLServiceFeeder::getService(sri.offline_index))){
                //!service returned online
                URLHASF_INFO << "Service " << sri.offline_url << " returned online";
                boost::unique_lock<boost::mutex> wr(mutex_queue);
                respawned_queue.push(sri.offline_index);
            } else {
                //service still in offline
                sri.retry_timeout = (sri.retry_timeout + 1000)%10000;
                
                URLHASF_INFO << "Service " << sri.offline_url << " still offline wait for " << sri.retry_timeout-current_ts << " milliseocnds";
                boost::unique_lock<boost::mutex> wr(mutex_queue);
                retry_queue.push(sri);
            }
        }
    }
}