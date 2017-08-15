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
