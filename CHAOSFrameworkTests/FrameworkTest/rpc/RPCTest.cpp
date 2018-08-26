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

RpcHandler::RpcHandler() {
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

CDataWrapper *RpcHandler::actionWithResult(CDataWrapper *action_data, bool& detach) {
    detach = true;
    return action_data;
}

CDataWrapper *RpcHandler::actionWithNoResult(CDataWrapper *action_data, bool& detach) {
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

TEST_F(RPCTest, ActionResult) {
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
    future = msg_chnl->sendRequestWithFuture("localhost:8888",
                                             "test_rpc",
                                             "actionWithNoResult",
                                             ChaosMoveOperator(pack));
    ASSERT_TRUE(future->wait(1000000));
    
    ASSERT_FALSE(future->getResult());
    NetworkBroker::getInstance()->deregisterAction(&rpc_handler);
}
