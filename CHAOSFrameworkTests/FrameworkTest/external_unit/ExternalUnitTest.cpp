/*
 *	ExternalUnitTest.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 13/10/2017 INFN, National Institute of Nuclear Physics
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

#include "ExternalUnitTest.h"

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::external_unit;


ExternalUnitTest::ExternalUnitTest():
ExternalUnitClientEndpoint("EchoTest"){}

ExternalUnitTest::~ExternalUnitTest(){}

void ExternalUnitTest::handleNewConnection(const std::string& connection_identifier) {
    
}

void ExternalUnitTest::handleDisconnection(const std::string& connection_identifier) {
    
}

int ExternalUnitTest::handleReceivedeMessage(const std::string& connection_identifier,
                                               chaos::common::data::CDWUniquePtr message) {
    return 0;
}

void ExternalUnitTest::SetUp(){}

TEST_F(ExternalUnitTest, InitDeinitTest) {
    for(int idx = 0; idx < 100; idx++) {
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->init(NULL));
        
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->deinit());
    }
}

TEST_F(ExternalUnitTest, EchoTest) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue("message", "this is the message");
    ASSERT_NO_THROW(InizializableService::initImplementation(ExternalUnitManager::getInstance(), NULL, "ExternalUnitManager", __PRETTY_FUNCTION__););
    ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                            "http",
                                                            "application/bson-json",
                                                            "ws://localhost:8080/echo");
    ASSERT_EQ(sendMessage(ChaosMoveOperator(message)),0);
    ExternalUnitManager::getInstance()->releaseConnection(*this);
    ASSERT_NO_THROW(InizializableService::deinitImplementation(ExternalUnitManager::getInstance(), "ExternalUnitManager", __PRETTY_FUNCTION__););
}
