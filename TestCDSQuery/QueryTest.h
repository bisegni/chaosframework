/*
 *	QueryTest.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__QueryTest__
#define __CHAOSFramework__QueryTest__
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>

class QueryTest : public chaos::ChaosCommon<QueryTest> {
    friend class Singleton<QueryTest>;
    chaos::common::utility::StartableServiceContainer<chaos::common::network::NetworkBroker> network_broker;
   chaos::common::io::IODirectIODriver *io_driver;
public:
    void init(void *init_data) throw (chaos::CException);
    void start() throw (chaos::CException);
    void stop() throw (chaos::CException);
    void deinit() throw (chaos::CException);
    void addURL(const std::string& direct_io_url);
    void doQuery(const std::string& producer_key,
                 const std::string& start_time,
                 const std::string& end_time);
};

#endif /* defined(__CHAOSFramework__QueryTest__) */
