//
//  main.cpp
//  CouchTest
//
//  Created by Claudio Bisegni on 21/03/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>

#include <chaos/common/utility/TimingUtil.h>

#include <libcouchbase/couchbase.h>

using namespace chaos::common::utility;

#define TEST_DURATION 60000

lcb_t					instance;
lcb_error_t				last_err;
struct lcb_create_st	create_options;

void errorCallback(lcb_t instance,
                   lcb_error_t err,
                   const char *errinfo) {
    (void)instance;
    last_err = err;
    if(errinfo) {
        std::cerr << "Error " << err << " with message " << errinfo <<std::endl;
    }
}

void getCallback(lcb_t instance,
                 const void *cookie,
                 lcb_error_t error,
                 const lcb_get_resp_t *resp) {
    (void)instance;
    if((last_err = error) == LCB_SUCCESS) {
        std::cout << "Got n:" << resp->v.v0.nbytes << " from server on key " << std::string((char*)resp->v.v0.key, resp->v.v0.nkey) << std::endl;
    }
}

int initCouchbase() {
    std::string all_server_str;
    std::cout << "Connect to all servers" << std::endl;
    if(instance) {
        lcb_destroy(instance);
        instance = NULL;
    }
    //clear the configuration
    memset(&create_options, 0, sizeof(create_options));
    
    //create_options
    create_options.version = 3;
    create_options.v.v3.username ="chaos";
    create_options.v.v3.passwd = "chaos";
    
    all_server_str.assign("couchbase://host;host");
    all_server_str.append("/");
    all_server_str.append("chaos");
    create_options.v.v3.connstr = all_server_str.c_str();
    
    //create the instance
    last_err = lcb_create(&instance, &create_options);
    if (last_err != LCB_SUCCESS) {
        std::cerr << "Error initializing the session params:\""<<create_options.v.v3.connstr<<"\" -> "<< lcb_strerror(NULL, last_err) << std::endl;
        return -1;
    } else {
        std::cout << "session params:"<<create_options.v.v3.connstr << std::endl;
    }
    
    lcb_U32 newval = 2000000; // Set to 4 seconds
    lcb_cntl(instance, LCB_CNTL_SET, LCB_CNTL_OP_TIMEOUT, &newval);
    
    lcb_behavior_set_syncmode(instance, LCB_SYNCHRONOUS);
    
    /* Set up the handler to catch all errors! */
    lcb_set_error_callback(instance, errorCallback);
    
    /* initiate the connect sequence in libcouchbase */
    last_err = lcb_connect(instance);
    if (last_err != LCB_SUCCESS) {
        std::cerr<< "Error connecting the session -> " << lcb_strerror(NULL, last_err) << std::endl;
        return -1;
    }
    
    /* run the event loop and wait until we've connected */
    last_err = lcb_wait(instance);
    
    if(last_err == LCB_SUCCESS) {
        std::cout << "connection ok" << std::endl;
        /* set up a callback for our get and set requests  */
        lcb_set_get_callback(instance, getCallback);
    } else {
        std::cerr << "Error connecting -> " << lcb_strerror(NULL, last_err) << std::endl;
        return -2;
        
    }
    lcb_size_t num_events = 0;
    lcb_cntl(instance, LCB_CNTL_GET, LCB_CNTL_CONFERRTHRESH, &num_events);
    num_events = 1;
    lcb_cntl(instance, LCB_CNTL_SET, LCB_CNTL_CONFERRTHRESH, &num_events);
    
    lcb_cntl(instance, LCB_CNTL_GET, LCB_CNTL_CONFDELAY_THRESH, &num_events);
    num_events = 500000;
    lcb_cntl(instance, LCB_CNTL_SET, LCB_CNTL_CONFDELAY_THRESH, &num_events);
    return last_err;
}

bool perforGetOperationOnKey(const std::string& key) {
    int err = LCB_SUCCESS;
    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *commands[1];
    
    commands[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = key.c_str();
    cmd.v.v0.nkey = key.size();
    err = lcb_get(instance, NULL, 1, commands);
    if (err != LCB_SUCCESS &&
        err != LCB_KEY_ENOENT) {
        std::cout << "Fail to get value "<<key<<" for with err "<< err << "(" << lcb_errmap_default(instance, err) << ")" << std::endl;
    } else {
        if(err == LCB_KEY_ENOENT) {
            err = 0;
        }
    }
    return err != LCB_SUCCESS;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    if(initCouchbase() != 0) return EXIT_FAILURE;
    
    bool work = true;
    uint32_t counter = 0;
    uint64_t start_work = TimingUtil::getTimeStamp();
    uint64_t start_stat_ts = start_work;
    uint64_t last_stat_ts = start_stat_ts;
    
    while(work) {
        start_stat_ts = TimingUtil::getTimeStamp();
        counter++;
        if((start_stat_ts - last_stat_ts) >= 1000) {
            std::cout << "Cicle stat = " << counter << std::endl;
            counter = 0;
            last_stat_ts = start_stat_ts;
        }
        
        work = ((start_stat_ts - start_work) < TEST_DURATION);
    }
    return EXIT_SUCCESS;
}
