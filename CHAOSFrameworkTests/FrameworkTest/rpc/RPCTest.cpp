//
//  RPCTest.cpp
//  FrameworkTest
//
//  Created by Claudio Bisegni on 11/07/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#include "RPCTest.h"

#include<chaos/common/global.h>
#include<chaos/common/chaos_constants.h>
#include<chaos/common/message/NodeMessageChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::message;

RpcHandler::RpcHandler():
actionWithResultCounter(0),
actionWithNoResultCounter(0){
    //register the action for the response
    DeclareAction::addActionDescritionInstance<RpcHandler>(this,
                                                           &RpcHandler::actionWithResult,
                                                           "test_rpc",
                                                           "actionWithResult",
                                                           "actionWithResult");
    DeclareAction::addActionDescritionInstance<RpcHandler>(this,
                                                           &RpcHandler::actionWithNoResult,
                                                           "test_rpc",
                                                           "actionWithNoResult",
                                                           "actionWithNoResult");
}

CDWUniquePtr RpcHandler::actionWithResult(CDWUniquePtr action_data) {
    actionWithResultCounter++;
    return action_data;
}

CDWUniquePtr RpcHandler::actionWithNoResult(CDWUniquePtr action_data) {
    actionWithNoResultCounter++;
    return NULL;
}


TEST_F(RPCTest, SendEcho) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    MessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMessageChannel();
    ChaosUniquePtr<MessageRequestFuture> future = msg_chnl->sendRequestWithFuture("localhost:8888",
                                                                                  chaos::NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                  chaos::NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                                                                  ChaosMoveOperator(pack));
    ASSERT_TRUE(future->wait(1000000));
    
    ASSERT_TRUE(future->getResult());
    ASSERT_EQ(future->getResult()->getStringValue("echo").compare("value"), 0);
}

TEST_F(RPCTest, SendRequest) {
    RpcHandler rpc_handler;
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    NetworkBroker::getInstance()->registerAction(&rpc_handler);
    MessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMessageChannel();
    ChaosUniquePtr<MessageRequestFuture> future = msg_chnl->sendRequestWithFuture("localhost:8888",
                                                                                  "test_rpc",
                                                                                  "actionWithResult",
                                                                                  ChaosMoveOperator(pack));
    ASSERT_TRUE(future->wait(1000000));
    ASSERT_TRUE(future->getResult());
    ASSERT_EQ(future->getResult()->getStringValue("echo").compare("value"), 0);
    ASSERT_EQ(rpc_handler.actionWithResultCounter, 1);
    
    future = msg_chnl->sendRequestWithFuture("localhost:8888",
                                             "test_rpc",
                                             "actionWithNoResult",
                                             ChaosMoveOperator(pack));
    ASSERT_TRUE(future->wait(1000000));
    ASSERT_FALSE(future->getResult());
    ASSERT_EQ(rpc_handler.actionWithNoResultCounter, 1);
    
    NetworkBroker::getInstance()->deregisterAction(&rpc_handler);
}

TEST_F(RPCTest, SendMessage) {
    unsigned int retry = 0;
    RpcHandler rpc_handler;
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    NetworkBroker::getInstance()->registerAction(&rpc_handler);
    MessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMessageChannel();
    msg_chnl->sendMessage("localhost:8888",
                          "test_rpc",
                          "actionWithResult",
                          ChaosMoveOperator(pack));

    msg_chnl->sendMessage("localhost:8888",
                         "test_rpc",
                         "actionWithNoResult",
                         ChaosMoveOperator(pack));
    while(rpc_handler.actionWithResultCounter != 1 &&
          rpc_handler.actionWithNoResultCounter != 1) {
        sleep(1);
        if(retry++ > 10){break;}
    }
    ASSERT_LE(retry, 10);
    ASSERT_EQ(rpc_handler.actionWithResultCounter, 1);
    ASSERT_EQ(rpc_handler.actionWithNoResultCounter, 1);
    NetworkBroker::getInstance()->deregisterAction(&rpc_handler);
}
