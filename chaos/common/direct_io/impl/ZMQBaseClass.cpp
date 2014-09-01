//
//  ZMQBaseClass.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/impl/ZMQBaseClass.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <string>
#include <zmq.h>

using namespace std;
using namespace chaos::common::direct_io::impl;

#if (defined (__WINDOWS__))
#   define randof(num)  (int) ((float) (num) * rand () / (RAND_MAX + 1.0))
#else
#   define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))
#endif

int ZMQBaseClass::closeSocketNoWhait (void *socket) {
    int linger = 0;
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(rc != 0 || errno != ETERM) {
        return rc;
    }
    return zmq_close (socket);
}
//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
char * ZMQBaseClass::stringReceive(void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    if (size > 255)
        size = 255;
    buffer [size] = 0;
    return strdup (buffer);
}

//  Convert C string to 0MQ string and send to socket
int ZMQBaseClass::stringSend(void *socket, const char *string) {
    int size = zmq_send (socket, string, strlen(string), 0);
    return size;
}

//  Sends string as 0MQ string, as multipart non-terminal
int ZMQBaseClass::stringSendMore(void *socket, const char *string) {
    int size = zmq_send (socket, string, strlen(string), ZMQ_SNDMORE);
    return size;
}

int ZMQBaseClass::setID(void *socket) {
	std::string uid = chaos::UUIDUtil::generateUUIDLite();
    return zmq_setsockopt (socket, ZMQ_IDENTITY, uid.c_str(), uid.size());
}

int ZMQBaseClass::setAndReturnID(void *socket, std::string& new_id) {
	new_id = chaos::UUIDUtil::generateUUIDLite();
    return zmq_setsockopt (socket, ZMQ_IDENTITY, new_id.c_str(), new_id.size());
}
