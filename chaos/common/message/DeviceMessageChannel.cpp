//
//  DeviceMessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "DeviceMessageChannel.h"

using namespace chaos;

int DeviceMessageChannel::initDevice(CDataWrapper *initData, uint32_t millisecToWait) {
    int err = 0;
    CHAOS_ASSERT(initData)
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "initDevice", initData, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT)){
        LDBG_ << "Init result: " << initResult->getJSONString();
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT);
    }
    return err;
}

int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "deinitDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT)){
        LDBG_ << "Init result: " << initResult->getJSONString();
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT);
    }
    return err;
}

int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "startDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT)){
        LDBG_ << "Init result: " << initResult->getJSONString();
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT);
    }
    return err;
}

int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "stopDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT)){
        LDBG_ << "Init result: " << initResult->getJSONString();
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT);
    }
    return err;
}