//
//  RpcClient.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "RpcClient.h"
#include <chaos/common/global.h>

using namespace chaos;

/*!
 Constructor di default per i
 */
RpcClient::RpcClient(const std::string& alias):NamedService(alias){
};

/*!
 manage the call for the handler when nd error occours
 */
inline void RpcClient::callErrorHandler(NetworkForwardInfo *fInfo, ErrorCode::ErrorCode eCode) {
    CHAOS_ASSERT(fInfo)
    if(fInfo->errorOpHandler) fInfo->errorOpHandler(fInfo->emitterIdentifier, fInfo->tag, eCode);
}

/*!
 manage the call for the handler when the operation has ended
 */
inline void RpcClient::callEndOpHandler(NetworkForwardInfo *fInfo) {
    CHAOS_ASSERT(fInfo)
    if(fInfo->errorOpHandler) fInfo->endOpHandler(fInfo->emitterIdentifier, fInfo->tag);
}