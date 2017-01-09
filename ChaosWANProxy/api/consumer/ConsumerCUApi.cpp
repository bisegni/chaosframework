/*
 *	ConsumerCUApi.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti.
 *
 *    	Copyrigh 2016 INFN, National Institute of Nuclear Physics
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
#include "ConsumerCUApi.h"

#include <cstring>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/ChaosCommon.h>
#include <boost/algorithm/string.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::consumer;

#define PRODUCER_INSERT_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_insert_err", err, "producer_insert_err_msg", msg)

#define PID_LAPP LAPP_ << "[ConsumerCUApi] - "
#define PID_LDBG LDBG_ << "[ConsumerCUApi] - "
#define PID_LERR LERR_ << "[ConsumerCUApi] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "
static boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));
static uint64_t pktid=1;
//! default constructor
ConsumerCUApi::ConsumerCUApi(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("CU",
            _persistence_driver){
    
}

//! default destructor
ConsumerCUApi::~ConsumerCUApi() {
    
}

//! execute the api
int ConsumerCUApi::execute(std::vector<std::string>& api_tokens,
                                   const Json::Value& input_data,
                                   std::map<std::string, std::string>& output_header,
                                   Json::Value& output_data) {
    CHAOS_ASSERT(persistence_driver)
    int err = 0;
    std::string err_msg;
    std::string producer_name;
    int cnt;
    uint64_t ts;
    if(api_tokens.size() == 0) {
        err_msg = "no producer name in the uri";
        PID_LERR << err_msg;
        PRODUCER_INSERT_ERR(output_data, -1, err_msg);
        return err;
    } /*else if(api_tokens.size() > 1) {
       err_msg = "too many param in the uri";
       PID_LERR << err_msg;
       
       PRODUCER_INSERT_ERR(output_data, -2, err_msg);
       return err;
       }*/
    for(cnt = 0;cnt<api_tokens.size();cnt++){
        
        if(cnt<api_tokens.size()-1){
            producer_name=producer_name + api_tokens[cnt] + "/";
        } else {
            producer_name=producer_name + api_tokens[cnt] ;
        }
    }
    
    
     return err;
}
