//
//  RpcServer.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "RpcServer.h"

using namespace chaos;

RpcServer::RpcServer(string *alias) {
    typeName = alias;
}

/*!
 Return the published port
 */
int RpcServer::getPublishedPort(){
    return portNumber;
}

/*
 set the command dispatcher associated to the instance of rpc adapter
 */
void RpcServer::setCommandDispatcher(RpcServerHanlder *newCommandHandler){
    commandHandler = newCommandHandler;
}

/*
 Return the adapter alias
 */
const char * RpcServer::getName() const {
    return typeName->c_str();
}