//
//  RpcClient.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "RpcClient.h"

using namespace chaos;

/*!
 Constructor di default per i
 */
RpcClient::RpcClient(string *alias){
    typeName = alias;
};

/*!
 Return the adapter alias
 */
const char * RpcClient::getName() const {
    return typeName->c_str();
}