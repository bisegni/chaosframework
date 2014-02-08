//
//  ZMQBaseClass.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/impl/ZMQBaseClass.h>

#include <zmq.h>

using namespace chaos::common::direct_io::impl;

int ZMQBaseClass::closeSocketNoWhait (void *socket) {
    int linger = 0;
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(rc != 0 || errno != ETERM) {
        return rc;
    }
    return zmq_close (socket);
}