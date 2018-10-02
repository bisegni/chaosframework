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
#pragma mark ServerEndpoint
ServerEndpoint::ServerEndpoint():
ExternalUnitServerEndpoint("/test"),
connection_event_counter(0),
disconnection_event_counter(0),
received_message_counter(0){
    connection_event_counter = 0;
    disconnection_event_counter = 0;
    received_message_counter = 0;
}

ServerEndpoint::~ServerEndpoint() {}

void ServerEndpoint::handleNewConnection(const std::string& connection_identifier) {
    connection_event_counter++;
}

void ServerEndpoint::handleDisconnection(const std::string& connection_identifier) {
    disconnection_event_counter++;
}

int ServerEndpoint::handleReceivedeMessage(const std::string& connection_identifier,
                                           chaos::common::data::CDWUniquePtr message) {
    received_message_counter++;
    return 0;
}

#pragma mark ExternalUnitTest
ExternalUnitTest::ExternalUnitTest():
ExternalUnitClientEndpoint("EchoTest"),
echo_received(false){}

ExternalUnitTest::~ExternalUnitTest(){}

void ExternalUnitTest::handleNewConnection(const std::string& connection_identifier) {current_connection = connection_identifier;connection_event_counter++;}

void ExternalUnitTest::handleDisconnection(const std::string& connection_identifier) {current_connection.clear();disconnection_event_counter++;}

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

TEST_F(ExternalUnitTest, InitDeinitCicle) {
    ASSERT_NO_THROW(ExternalUnitManager::getInstance()->deinit());
    for(int idx = 0; idx < 100; idx++) {
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->init(NULL));
        
        ASSERT_NO_THROW(ExternalUnitManager::getInstance()->deinit());
    }
    ASSERT_NO_THROW(ExternalUnitManager::getInstance()->init(NULL));
}

TEST_F(ExternalUnitTest, WrongEndpoint) {
    int retry = 0;
    
    ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                            "http",
                                                            "application/bson-json",
                                                            "ws://localhost:8080/badendpoint");
    while(ExternalUnitClientEndpoint::isOnline() == false){
        ASSERT_LE(retry++, 1000);
        usleep(500000);
    }
    while(ExternalUnitClientEndpoint::getAcceptedState() != -1){
        ASSERT_LE(retry++, 1000);
        usleep(500000);
    }

    while(ExternalUnitClientEndpoint::isOnline() == true){
        ASSERT_LE(retry++, 1000);
        usleep(500000);
    }
    
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), false);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), -1);
    ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                          "http");

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
        ASSERT_LE(retry++, 1000);
        usleep(500000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), true);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), 1);
    
    //send echo message
    ASSERT_EQ(sendMessage(current_connection, ChaosMoveOperator(message)),0);
    //wait answer
    retry = 0;
    while(echo_received == false) {
        ASSERT_LE(retry++, 10);
        usleep(500000);
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
    ServerEndpoint test_endpoint;
    ASSERT_EQ(ExternalUnitManager::getInstance()->registerEndpoint(test_endpoint), 0);
    
    ASSERT_EQ(ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                                      "http",
                                                                      "application/bson-json",
                                                                      "ws://localhost:8080/test"), 0);
    //wait connection
    while(ExternalUnitClientEndpoint::isOnline() == false ||
          ExternalUnitClientEndpoint::getAcceptedState() != 1) {
        ASSERT_LE(retry++, 10);
        usleep(500000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), true);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), 1);
    
    //discnnect server endpoint
    ASSERT_EQ(ExternalUnitManager::getInstance()->deregisterEndpoint(test_endpoint), 0);
    while(ExternalUnitClientEndpoint::isOnline() != false ||
          ExternalUnitClientEndpoint::getAcceptedState() != -1) {
        ASSERT_LE(retry++, 10);
        usleep(500000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), false);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), -1);
    
    //!register server endpoint again
    ASSERT_EQ(ExternalUnitManager::getInstance()->registerEndpoint(test_endpoint), 0);
    //wait connection
    while(ExternalUnitClientEndpoint::isOnline() == false ||
          ExternalUnitClientEndpoint::getAcceptedState() != 1) {
        ASSERT_LE(retry++, 60);
        usleep(1000000);
    }
    ASSERT_EQ(ExternalUnitManager::getInstance()->deregisterEndpoint(test_endpoint), 0);
    sleep(1);
    ASSERT_EQ(ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                                    "http"), 0);
    ASSERT_EQ(connection_event_counter, 1);
    ASSERT_EQ(received_message_counter, 0);
    ASSERT_EQ(disconnection_event_counter, 1);
    
    ASSERT_EQ(test_endpoint.connection_event_counter, 2);
    ASSERT_EQ(test_endpoint.received_message_counter, 0);
    ASSERT_EQ(test_endpoint.disconnection_event_counter, 2);
}
