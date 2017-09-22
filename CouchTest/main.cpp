/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

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
        // std::cout << "Got n:" << resp->v.v0.nbytes << " from server on key " << std::string((char*)resp->v.v0.key, resp->v.v0.nkey) << std::endl;
    } else if(error == LCB_KEY_ENOENT) {
        std::cerr << "key " << std::string((char*)resp->v.v0.key, resp->v.v0.nkey) << " not found "<< std::endl;
    }
}

void setCallback(lcb_t instance,
                 const void *cookie,
                 lcb_storage_t operation,
                 lcb_error_t error,
                 const lcb_store_resp_t *resp) {
    (void)instance;
    if (error != LCB_SUCCESS) {
        std::cerr << "key " << std::string((char*)resp->v.v0.key, resp->v.v0.nkey) << " not set "<< std::endl;
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
    all_server_str.assign("couchbase://chaosdev-cb1.chaos.lnf.infn.it;chaosdev-cb2.chaos.lnf.infn.it");
    //all_server_str.assign("couchbase://macbisegni");
    //all_server_str.assign("couchbase://vldantemng001.lnf.infn.it");
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
        lcb_set_store_callback(instance, setCallback);
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
    lcb_error_t err = LCB_SUCCESS;
    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *commands[1];
    
    commands[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = key.c_str();
    cmd.v.v0.nkey = key.size();
    err = lcb_get(instance, NULL, 1, commands);
    err = lcb_wait(instance);
    if (err != LCB_SUCCESS &&
        err != LCB_KEY_ENOENT) {
        std::cout << "Fail to get value "<<key<<" for with err "<< err << "(" << lcb_errmap_default(instance, err) << ")" << std::endl;
    } else {
        if(err == LCB_KEY_ENOENT) {
            err = LCB_SUCCESS;
        }
    }
    return err != LCB_SUCCESS;
}

int perforSetOperationOnKey(const std::string& key, const std::string& value) {
    lcb_error_t err = LCB_SUCCESS;
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t * const commands[] = { &cmd };
    
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = key.c_str();
    cmd.v.v0.nkey = key.size();
    cmd.v.v0.bytes = value.c_str();
    cmd.v.v0.nbytes = value.size();
    cmd.v.v0.operation = LCB_SET;
    err = lcb_store(instance, NULL, 1, commands);
    err = lcb_wait(instance);
    if (err != LCB_SUCCESS) {
        std::cout << "Fail to store value "<<key<<" for with err "<< err << "(" << lcb_errmap_default(instance, err) << ")" << std::endl;
    }
    return err;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    if(initCouchbase() != 0) return EXIT_FAILURE;
    
    bool work = true;
    uint32_t counter = 0;
    uint64_t start_work = TimingUtil::getTimeStamp();
    uint64_t start_stat_ts = start_work;
    uint64_t last_stat_ts = start_stat_ts;
    std::string key = "rt_sin_e_o";
    perforSetOperationOnKey(key, "value for key");
    while(work) {
        start_stat_ts = TimingUtil::getTimeStamp();
        perforGetOperationOnKey(key);
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
