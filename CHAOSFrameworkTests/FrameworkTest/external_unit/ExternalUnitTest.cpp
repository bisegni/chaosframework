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

#define ECHO_TEST_KEY       "message"
#define ECHO_KEY_MESSAGE    "this is the message"

ServerEndpoint::ServerEndpoint() {
    connection_event_counter = 0;
    disconnection_event_counter = 0;
    received_message_counter = 0;
}

ServerEndpoint::~ServerEndpoint() {
    
}

void ServerEndpoint::handleNewConnection(const std::string& connection_identifier) {connection_event_counter++;}

void ServerEndpoint::handleDisconnection(const std::string& connection_identifier) {disconnection_event_counter++;}

int ServerEndpoint::handleReceivedeMessage(const std::string& connection_identifier,
                                           chaos::common::data::CDWUniquePtr message) {
    received_message_counter++;
    return 0;
}



ExternalUnitTest::ExternalUnitTest():
ExternalUnitClientEndpoint("EchoTest"){}

ExternalUnitTest::~ExternalUnitTest(){}

void ExternalUnitTest::handleNewConnection(const std::string& connection_identifier) {connection_event_counter++;}

void ExternalUnitTest::handleDisconnection(const std::string& connection_identifier) {disconnection_event_counter++;}

int ExternalUnitTest::handleReceivedeMessage(const std::string& connection_identifier,
                                             chaos::common::data::CDWUniquePtr message) {
    received_message_counter++;
    echo_received = message->hasKey(ECHO_TEST_KEY) &&
    message->isStringValue(ECHO_TEST_KEY) &&
    (message->getStringValue(ECHO_TEST_KEY).compare(ECHO_KEY_MESSAGE) == 0);
    return 0;
}

void ExternalUnitTest::SetUp() {
    echo_received = false;
    connection_event_counter = 0;
    received_message_counter = 0;
    disconnection_event_counter = 0;
    ASSERT_NO_THROW(InizializableService::initImplementation(ExternalUnitManager::getInstance(), NULL, "ExternalUnitManager", __PRETTY_FUNCTION__););
}

void ExternalUnitTest::TearDown() {
    ASSERT_NO_THROW(InizializableService::deinitImplementation(ExternalUnitManager::getInstance(), "ExternalUnitManager", __PRETTY_FUNCTION__););
}

TEST(ExternalUnitTest, InitDeinitCicle) {
    for(int idx = 0; idx < 100; idx++) {
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->init(NULL));
        
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->deinit());
    }
}

TEST_F(ExternalUnitTest, Echo) {
    int retry = 0;
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(ECHO_TEST_KEY, ECHO_KEY_MESSAGE);
    
    ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                            "http",
                                                            "application/bson-json",
                                                            "ws://localhost:8080/echo");
    while(ExternalUnitClientEndpoint::isOnline() == false ||
          ExternalUnitClientEndpoint::getAcceptedState() != 1) {
        ASSERT_LE(retry++, 3);
        usleep(1000000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), true);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), 1);
    
    //send echo message
    ASSERT_EQ(sendMessage(ChaosMoveOperator(message)),0);
    //wait answer
    retry = 0;
    while(echo_received == false) {
        ASSERT_LE(retry++, 3);
        usleep(1000000);
    }
    ASSERT_TRUE(echo_received);
    ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                          "http");
    ASSERT_EQ(connection_event_counter, 1);
    ASSERT_EQ(received_message_counter, 1);
    ASSERT_EQ(disconnection_event_counter, 1);
}

TEST_F(ExternalUnitTest, Reconnection) {
    unsigned int retry = 0;
    
    ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                            "http",
                                                            "application/bson-json",
                                                            "ws://localhost:8080/echo");
    while(ExternalUnitClientEndpoint::isOnline() == false ||
          ExternalUnitClientEndpoint::getAcceptedState() != 1) {
        ASSERT_LE(retry++, 3);
        usleep(1000000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), true);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), 1);
    
    //wait answer
    retry = 0;
    while(echo_received == false) {
        ASSERT_LE(retry++, 3);
        usleep(1000000);
    }
    ASSERT_TRUE(echo_received);
    ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                          "http");
    ASSERT_EQ(connection_event_counter, 1);
    ASSERT_EQ(received_message_counter, 1);
    ASSERT_EQ(disconnection_event_counter, 1);
}
