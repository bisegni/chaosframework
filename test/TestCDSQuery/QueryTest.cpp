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

#include "QueryTest.h"
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::common::data;

void QueryTest::init(void *init_data) throw (chaos::CException) {
    ChaosCommon<QueryTest>::init(init_data);
    
    network_broker.reset(NetworkBroker::getInstance(), "NetworkBroker");
    network_broker.init(init_data, __PRETTY_FUNCTION__);
    
    chaos::common::io::IODirectIODriverInitParam io_driver_param;
    std::memset(&io_driver_param, 0, sizeof(chaos::common::io::IODirectIODriverInitParam));
    io_driver_param.network_broker = network_broker.get();
    
    io_driver = new chaos::common::io::IODirectIODriver("IODirectIODriver");
    io_driver->setDirectIOParam(io_driver_param);
    io_driver->init(NULL);
    
}

void QueryTest::start() throw (chaos::CException) {
    network_broker.start(__PRETTY_FUNCTION__);
}

void QueryTest::stop() throw (chaos::CException) {
    network_broker.stop(__PRETTY_FUNCTION__);
}

void QueryTest::deinit() throw (chaos::CException) {
    if(io_driver) {
        io_driver->deinit();
        delete(io_driver);
    }
    network_broker.deinit(__PRETTY_FUNCTION__);
}

void QueryTest::addURL(const std::string& direct_io_url) {
     io_driver->addServerURL(direct_io_url);
}

void QueryTest::doQuery(const std::string& producer_key,
                        const std::string& start_time,
                        const std::string& end_time) {
    uint64_t start_ts = start_time.size()>0?chaos::common::utility::TimingUtil::getTimestampFromString(start_time):0;
    uint64_t end_ts = end_time.size()>0?chaos::common::utility::TimingUtil::getTimestampFromString(end_time):0;
   
    
    chaos::common::io::QueryFuture * query_future = io_driver->performQuery(producer_key,
                                                                            start_ts,
                                                                            end_ts);
    if(query_future) {
        query_future->waitForBeginResult();
        while(query_future->getState() ==chaos::common::io::QueryFutureStateStartResult ||
              query_future->getState() ==chaos::common::io::QueryFutureStateReceivingResult){
            auto_ptr<CDataWrapper> q_result(query_future->getDataPack(true, 100000));
            if(q_result.get()) {
                std::cout << q_result->getJSONString() << std::endl;
            } else {
                break;
            }
        };

        std::cout << "Releasing query" << std::endl;
        io_driver->releaseQuery(query_future);
    }

}