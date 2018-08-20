/*
 *	ExternalUnitMulticlientTest.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 18/03/2018 INFN, National Institute of Nuclear Physics
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

#include <gtest/gtest.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/external_unit/external_unit.h>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::external_unit;

#define ECHO_TEST_KEY           "message"
#define NUMBER_OF_CLIENT        200
#define RUN_NUMBER_OF_SEND_DATA 10

boost::atomic<int64_t> ended(0);

#pragma mark ExternalUnitMulticlientTestDeclare
class ExternalUnitMulticlientTest:
public testing::Test {
protected:
public:
    ExternalUnitMulticlientTest();
    ~ExternalUnitMulticlientTest();
    void SetUp();
    void TearDown();
};
#pragma mark ExternalUnitMulticlientTestDefine
ExternalUnitMulticlientTest::ExternalUnitMulticlientTest(){}

ExternalUnitMulticlientTest::~ExternalUnitMulticlientTest(){}

void ExternalUnitMulticlientTest::SetUp() {
    ASSERT_NO_THROW(InizializableService::initImplementation(ExternalUnitManager::getInstance(), NULL, "ExternalUnitManager", __PRETTY_FUNCTION__););
}

void ExternalUnitMulticlientTest::TearDown() {
    ASSERT_NO_THROW(InizializableService::deinitImplementation(ExternalUnitManager::getInstance(), "ExternalUnitManager", __PRETTY_FUNCTION__););
}

#pragma mark ClientDeclare
class TestClient:
public chaos::common::external_unit::ExternalUnitClientEndpoint {
    //!notify a new arrived connection
    void handleNewConnection(const std::string& connection_identifier);
    
    //!notify that a connection has been closed
    void handleDisconnection(const std::string& connection_identifier);
    
    //! notify that a message has been received for a remote connection
    int handleReceivedeMessage(const std::string& connection_identifier,
                               chaos::common::data::CDWUniquePtr message);
protected:
    int client_id;
    bool echo_received;
    std::string current_connection;
    unsigned int connection_event_counter;
    unsigned int disconnection_event_counter;
    unsigned int received_message_counter;
public:
    TestClient(int _client_id);
    ~TestClient();
    void run();
};

#pragma mark ClientDefine
TestClient::TestClient(int _client_id):
ExternalUnitClientEndpoint("Client"),
client_id(_client_id){
    echo_received = false;
    connection_event_counter = 0;
    received_message_counter = 0;
    disconnection_event_counter = 0;
}

TestClient::~TestClient(){}
void TestClient::handleNewConnection(const std::string& connection_identifier){current_connection = connection_identifier; connection_event_counter++;}
void TestClient::handleDisconnection(const std::string& connection_identifier){current_connection.clear(); disconnection_event_counter++;}
int TestClient::handleReceivedeMessage(const std::string& connection_identifier,
                                             chaos::common::data::CDWUniquePtr message) {
    if(message->hasKey(ECHO_TEST_KEY) &&
       message->isInt32Value(ECHO_TEST_KEY) &&
       (message->getInt32Value(ECHO_TEST_KEY) == client_id)) {
        echo_received = true;
        received_message_counter++;
    }
    return 0;
}

void TestClient::run() {
    int retry = 0;
    ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                            "http",
                                                            "application/bson-json",
                                                            "ws://localhost:8080/echo");
    while(ExternalUnitClientEndpoint::isOnline() == false ||
          ExternalUnitClientEndpoint::getAcceptedState() != 1) {
        ASSERT_LE(retry++, 240);
        usleep(500000);
    }
    ASSERT_EQ(ExternalUnitClientEndpoint::isOnline(), true);
    ASSERT_EQ(ExternalUnitClientEndpoint::getAcceptedState(), 1);
    
    //send echo message
    for(int idx = 0; idx < RUN_NUMBER_OF_SEND_DATA; idx++) {
        CDWUniquePtr message(new CDataWrapper());
        message->addInt32Value(ECHO_TEST_KEY, client_id);
        ASSERT_EQ(sendMessage(current_connection, ChaosMoveOperator(message)),0);
    }
    
    //wait answer
    retry = 0;
    while(echo_received == false) {
        ASSERT_LE(retry++, 240);
        usleep(500000);
    }
    while(received_message_counter != RUN_NUMBER_OF_SEND_DATA) {
        ASSERT_LE(retry++, 240);
        usleep(500000);
    }
    ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                          "http");
    ASSERT_EQ(connection_event_counter, 1);
    ASSERT_EQ(received_message_counter, RUN_NUMBER_OF_SEND_DATA);
    ASSERT_EQ(disconnection_event_counter, 1);
    ended++;
}

void clientRunner(ChaosSharedPtr<TestClient> client) {
    client->run();
}

TEST_F(ExternalUnitMulticlientTest, MultithreadingTest) {
    boost::thread_group tg;
    for(int idx = 0; idx < NUMBER_OF_CLIENT; idx++) {
        tg.add_thread(new boost::thread(clientRunner, ChaosSharedPtr<TestClient>(new TestClient(idx))));
    }
    tg.join_all();
    ASSERT_EQ(ended, NUMBER_OF_CLIENT);
}
