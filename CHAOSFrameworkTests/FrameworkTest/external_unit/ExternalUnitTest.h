/*
 *	ExternalUnitTest.h
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

#ifndef ExternalUnitTest_h
#define ExternalUnitTest_h

#include <gtest/gtest.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/external_unit/external_unit.h>

class ServerEndpoint:
public chaos::common::external_unit::ExternalUnitServerEndpoint {
protected:
    void handleNewConnection(const std::string& connection_identifier);
    
    void handleDisconnection(const std::string& connection_identifier);
    
    int handleReceivedeMessage(const std::string& connection_identifier,
                               chaos::common::data::CDWUniquePtr message);
public:
    unsigned int connection_event_counter;
    unsigned int disconnection_event_counter;
    unsigned int received_message_counter;
    ServerEndpoint();
    ~ServerEndpoint();
};

class ExternalUnitTest:
public testing::Test,
public chaos::common::external_unit::ExternalUnitClientEndpoint {
protected:
    //!notify a new arrived connection
    void handleNewConnection(const std::string& connection_identifier);
    
    //!notify that a connection has been closed
    void handleDisconnection(const std::string& connection_identifier);
    
    //! notify that a message has been received for a remote connection
    int handleReceivedeMessage(const std::string& connection_identifier,
                               chaos::common::data::CDWUniquePtr message);
protected:
    bool echo_received;
    std::string current_connection;
    unsigned int connection_event_counter;
    unsigned int disconnection_event_counter;
    unsigned int received_message_counter;
public:
    ExternalUnitTest();
    ~ExternalUnitTest();
    void SetUp();
    void TearDown();
};

#endif /* ExternalUnitTest_h */
