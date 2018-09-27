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

#include "CouchbaseCacheDriver.h"
#include "../ChaosMetadataService.h"

#include <chaos/common/global.h>

#include <string>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#define CouchbaseCacheDriver_LOG_HEAD "[CouchbaseCacheDriver] - "

#define CCDLAPP_ LAPP_ << CouchbaseCacheDriver_LOG_HEAD
#define CCDLDBG_ LDBG_ << CouchbaseCacheDriver_LOG_HEAD << __PRETTY_FUNCTION__ << " - "
#define CCDLERR_ LERR_ << CouchbaseCacheDriver_LOG_HEAD

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::constants;
using namespace chaos::metadata_service::cache_system;

DEFINE_CLASS_FACTORY(CouchbaseCacheDriver, CacheDriver);

#pragma mark Callback Defintion
void CouchbaseCacheDriver::errorCallback(lcb_t instance,
                                         lcb_error_t err,
                                         const char *errinfo) {
    (void)instance;
}

void CouchbaseCacheDriver::getCallback(lcb_t instance,
                                       const void *cookie,
                                       lcb_error_t error,
                                       const lcb_get_resp_t *resp) {
    const Result *result = reinterpret_cast<const Result*>(cookie);
    
    switch(result->return_type) {
        case ResultTypeGet: {
            const GetResult * gr_ptr = dynamic_cast<const GetResult *>(result);
            CHAOS_ASSERT(gr_ptr);
            if((result->err = error) != LCB_SUCCESS) {
                result->err_str = lcb_strerror(instance, error);
            }else {
                gr_ptr->cached_data = ChaosMakeSharedPtr<Buffer>(resp->v.v0.bytes, resp->v.v0.nbytes);
            }
            break;
        }
            
        case ResultTypeMultiGet: {
            const MultiGetResult * gr_ptr = dynamic_cast<const MultiGetResult *>(result);
            CHAOS_ASSERT(gr_ptr);
            if((result->err = error) != LCB_SUCCESS) {
                result->err_str = lcb_strerror(instance, error);
                gr_ptr->multi_cached_data.insert(MultiCacheDataPair(std::string((char*)resp->v.v0.key,
                                                                                resp->v.v0.nkey),
                                                                    CacheData()));
            }else {
                gr_ptr->multi_cached_data.insert(MultiCacheDataPair(std::string((char*)resp->v.v0.key,
                                                                                resp->v.v0.nkey),
                                                                    ChaosMakeSharedPtr<Buffer>(resp->v.v0.bytes, resp->v.v0.nbytes)));
            }
            break;
        }
            
        default:
            break;
    }
}

void CouchbaseCacheDriver::setCallback(lcb_t instance,
                                       const void *cookie,
                                       lcb_storage_t operation,
                                       lcb_error_t error,
                                       const lcb_store_resp_t *resp) {
    const Result *result = reinterpret_cast<const Result*>(cookie);
    if((result->err = error) != LCB_SUCCESS) {
        result->err_str = lcb_strerror(instance, error);
        CCDLERR_ << result->err_str;
    }
}

#pragma mark Results Defintion
Result::Result(ResultType _return_type):
return_type(_return_type),
err(LCB_SUCCESS),
err_str(){}
Result::~Result(){}

GetResult::GetResult(CacheData& _cached_data):
Result(ResultTypeGet),
cached_data(_cached_data){}

MultiGetResult::MultiGetResult(MultiCacheData& _multi_cached_data):
Result(ResultTypeMultiGet),
multi_cached_data(_multi_cached_data){}

StoreResult::StoreResult():
Result(ResultTypeStore){}

#pragma mark Driver Definitions
CouchbaseCacheDriver::CouchbaseCacheDriver(std::string alias):
CacheDriver(alias){
    lcb_uint32_t ver;
    const char *msg = lcb_get_version(&ver);
    CCDLAPP_ << "Couchbase sdk version: " << msg;
}

CouchbaseCacheDriver::~CouchbaseCacheDriver() {}

//! init
void CouchbaseCacheDriver::init(void *init_data)  {
    //call superclass init
    CacheDriver::init(init_data);
    InizializableService::initImplementation(driver_pool, NULL, "CouchbaseDriverPool", __PRETTY_FUNCTION__);
}

//!deinit
void CouchbaseCacheDriver::deinit()  {
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(driver_pool, "CouchbaseDriverPool", __PRETTY_FUNCTION__);)
    //call superclass deinit
    CacheDriver::deinit();
}

int CouchbaseCacheDriver::putData(const std::string& key,
                                  CacheData data_to_store) {
    CHAOS_ASSERT(getServiceState() == CUStateKey::INIT)
    int err = 0;
    StoreResult result_wrap;
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t * const commands[] = { &cmd };
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = key.c_str();
    cmd.v.v0.nkey = key.size();
    cmd.v.v0.bytes = data_to_store->data();
    cmd.v.v0.nbytes = data_to_store->size();
    cmd.v.v0.operation = LCB_SET;
    //!get new instance
    CouchbasePoolSlot *pool_element = driver_pool.pool->getNewResource();
    if(pool_element) {
        err = lcb_store(*pool_element->resource_pooled, &result_wrap, 1, commands);
        err = lcb_wait(*pool_element->resource_pooled);
        if (result_wrap.err != LCB_SUCCESS) {
            CCDLERR_<< "Fail to set value -> "<< result_wrap.err_str;
        }
        err = result_wrap.err;
    } else {
        err = -1;
    }
    driver_pool.pool->releaseResource(pool_element);
    return err;
}

int CouchbaseCacheDriver::getData(const std::string& key,
                                  CacheData& cached_data) {
    CHAOS_ASSERT(getServiceState() == CUStateKey::INIT)
    int err = 0;
    GetResult result_wrap(cached_data);
    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *commands[1];
    commands[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = key.c_str();
    cmd.v.v0.nkey = key.size();
    
    CouchbasePoolSlot *pool_element = driver_pool.pool->getNewResource();
    if(pool_element) {
        err = lcb_get(*pool_element->resource_pooled, &result_wrap, 1, commands);
        err = lcb_wait(*pool_element->resource_pooled);
        if (result_wrap.err != LCB_SUCCESS &&
            result_wrap.err != LCB_KEY_ENOENT) {
            CCDLERR_<< "Fail to get value "<< key <<" for with err "<< result_wrap.err << "(" << result_wrap.err_str << ")";
        } else {
            if(result_wrap.err == LCB_KEY_ENOENT) {
                result_wrap.err = LCB_SUCCESS;
            }
        }
        err = result_wrap.err;
    } else {
        err = -1;
    }
    driver_pool.pool->releaseResource(pool_element);
    return err;
}

int CouchbaseCacheDriver::getData(const ChaosStringVector& keys,
                                  MultiCacheData& multi_data) {
    //crate vrapper result
    int err = 0;
    CouchbasePoolSlot *pool_element = driver_pool.pool->getNewResource();
    if(pool_element) {
        MultiGetResult result_wrap(multi_data);
        for(ChaosStringVectorConstIterator it = keys.begin(),
            end = keys.end();
            it != end;
            it++) {
            
            lcb_get_cmd_t cmd;
            const lcb_get_cmd_t *commands[1];
            commands[0] = &cmd;
            memset(&cmd, 0, sizeof(cmd));
            cmd.v.v0.key = it->c_str();
            cmd.v.v0.nkey = it->size();
            lcb_get(*pool_element->resource_pooled, &result_wrap, 1, commands);
        }
        lcb_wait(*pool_element->resource_pooled);
        err = 0;
    } else {
        err = -1;
    }
    return err;
}

int CouchbaseCacheDriver::addServer(const std::string& server_desc) {
    return driver_pool.addServer(server_desc);
}

int CouchbaseCacheDriver::removeServer(const std::string& server_desc) {
    return driver_pool.removeServer(server_desc);
}


#pragma mark CachePool
//-------------------------------------------cache pool---------------------------------------

static void couchbaseInstanceDeallocator(lcb_t *cb_ist) {
    if(cb_ist){}
}

CouchbaseDriverPool::CouchbaseDriverPool():
instance_created(0),
minimum_instance_in_pool(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_pool_min_instances_number) {
    
    all_server_to_use = ChaosMetadataService::getInstance()->setting.cache_driver_setting.startup_chache_servers;
    
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param.count("bucket")) {
        bucket_name = ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param["bucket"];
    }
    
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param.count("user")) {
        bucket_user = ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param["user"];
    }
    
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param.count("pwd")) {
        bucket_pwd = ChaosMetadataService::getInstance()->setting.cache_driver_setting.key_value_custom_param["pwd"];
    }
    
    pool.reset(new CouchbasePool("couchbase_cache_driver",
                                 this,
                                 minimum_instance_in_pool));
}

CouchbaseDriverPool::~CouchbaseDriverPool() {}

lcb_t* CouchbaseDriverPool::allocateResource(const std::string& pool_identification,
                                             uint32_t& alive_for_ms) {
    ChaosUniquePtr<lcb_t>       new_instance;
    struct lcb_create_st        create_options;
    lcb_error_t                 last_err = LCB_SUCCESS;
    std::string                 all_server_str;
    
    DEBUG_CODE(CCDLAPP_ << "New pool request allocation for couchbase driver";)
    //increment and check instance created
    if(instance_created+1 > minimum_instance_in_pool) {
        alive_for_ms = 1000*60*10; //one hour
    } else {
        //we want at least two active driver instance
        alive_for_ms = std::numeric_limits<uint32_t>::max();
    }
    
    try{
        new_instance.reset(new lcb_t());
        
        //clear the configuration
        memset(&create_options, 0, sizeof(create_options));
        
        //create_options
        create_options.version = 3;
        if(bucket_user.size()) create_options.v.v3.username = bucket_user.c_str();
        if(bucket_pwd.size()) create_options.v.v3.passwd = bucket_pwd.c_str();
        
        all_server_str.assign("couchbase://");
        for (ChaosStringVectorIterator iter = all_server_to_use.begin();
             iter!=all_server_to_use.end();
             iter++) {
            CCDLAPP_ << "Connecting to \""<<*iter<<"\"...";
            all_server_str.append(*iter);
            all_server_str.append(";");
        }
        if(all_server_str.size()) {
            //remove the last ';' character
            all_server_str.resize(all_server_str.size()-1);
        }
        all_server_str.append("/");
        all_server_str.append(bucket_name);
        CCDLAPP_ << "Create new session";
        //assign the host string to the configuration
        create_options.v.v3.connstr = all_server_str.c_str();
        
        //create the instance
        last_err = lcb_create(new_instance.get(), &create_options);
        if (last_err != LCB_SUCCESS) {
            throw CException(-1, CHAOS_FORMAT("Error initializing the session params: %1% - %2%", %create_options.v.v3.connstr%lcb_strerror(NULL, last_err)), __PRETTY_FUNCTION__);
        } else {
            CCDLDBG_ << "session params:"<<create_options.v.v3.connstr;
        }
        
        lcb_U32 newval = COUCHBASE_DEFAULT_TIMEOUT;
        if((last_err=lcb_cntl(*new_instance, LCB_CNTL_SET, LCB_CNTL_OP_TIMEOUT, &newval))!=LCB_SUCCESS){
            CCDLERR_<< "Cannot set OP Timeout -> " << lcb_strerror(NULL, last_err);
            
        }
        
        lcb_set_cookie(*new_instance, this);
        
        //lcb_behavior_set_syncmode(instance, LCB_SYNCHRONOUS);
        
        /* initiate the connect sequence in libcouchbase */
        last_err = lcb_connect(*new_instance);
        if (last_err != LCB_SUCCESS) {
            throw CException(-1, CHAOS_FORMAT("Error connecting the session -> ", %lcb_strerror(NULL, last_err)), __PRETTY_FUNCTION__);
        }
        
        /* Set up the handler to catch all errors! */
        //lcb_set_error_callback(*new_instance, CouchbaseCacheDriver::errorCallback);
        /* run the event loop and wait until we've connected */
        last_err = lcb_wait(*new_instance);
        
        if(last_err == LCB_SUCCESS) {
            CCDLDBG_ << "connection ok";
            /* set up a callback for our get and set requests  */
            lcb_set_get_callback(*new_instance, CouchbaseCacheDriver::getCallback);
            lcb_set_store_callback(*new_instance, CouchbaseCacheDriver::setCallback);
        } else {
            throw CException(-1, CHAOS_FORMAT("Error connecting the session -> ", %lcb_strerror(NULL, last_err)), __PRETTY_FUNCTION__);
            
        }
        lcb_size_t num_events = 0;
        lcb_cntl(*new_instance, LCB_CNTL_GET, LCB_CNTL_CONFERRTHRESH, &num_events);
        num_events = 1;
        lcb_cntl(*new_instance, LCB_CNTL_SET, LCB_CNTL_CONFERRTHRESH, &num_events);
        
        lcb_cntl(*new_instance, LCB_CNTL_GET, LCB_CNTL_CONFDELAY_THRESH, &num_events);
        num_events = ChacheTimeoutinMSec;
        lcb_cntl(*new_instance, LCB_CNTL_SET, LCB_CNTL_CONFDELAY_THRESH, &num_events);
        
    } catch(chaos::CException& ex) {
        DEBUG_CODE(CCDLERR_ << CHAOS_FORMAT("Error allocating new cache instance", %ex.what());)
        lcb_destroy(*new_instance);
        new_instance.reset();
    } catch(...) {
        DEBUG_CODE(CCDLERR_ << "Generic error allocating new cache instance";)
        lcb_destroy(*new_instance);
        new_instance.reset();
    }
    return new_instance.release();
}

void CouchbaseDriverPool::deallocateResource(const std::string& pool_identification,
                                             lcb_t *pooled_driver) {
    //decrement instance created
    instance_created--;
    lcb_destroy(*pooled_driver);
    delete(pooled_driver);
}

void CouchbaseDriverPool::init(void *init_data)  {}

void CouchbaseDriverPool::deinit()  {}

bool CouchbaseDriverPool::validateString(std::string& server_description) {
    boost::algorithm::trim(server_description);
    std::string normalized_server_desc = boost::algorithm::to_lower_copy(server_description);
    //! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
    boost::regex CouchbaseHostNameOnlyRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+");
    //! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
    boost::regex CouchbaseHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}");
    //! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
    boost::regex CouchbaseIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}");
    //check if the description is well formed
    if(!regex_match(normalized_server_desc, CouchbaseHostNameOnlyRegExp) &&
       !regex_match(normalized_server_desc, CouchbaseHostNameRegExp) &&
       !regex_match(normalized_server_desc, CouchbaseIPAndPortRegExp)) return false;
    return true;
}

int CouchbaseDriverPool::addServer(std::string server_desc) {
    if(!validateString(server_desc)) return -1;
    
    ChaosStringVectorIterator server_iter = std::find(all_server_to_use.begin(), all_server_to_use.end(), server_desc);
    if(server_iter != all_server_to_use.end()) {
        return -2;
    }
    //add new server
    CCDLAPP_ << "Add server "<< server_desc;
    all_server_to_use.push_back(server_desc);
    return 0;
}

int CouchbaseDriverPool::removeServer(std::string server_desc) {
    if(!validateString(server_desc)) return -1;
    ChaosStringVectorIterator server_iter = std::find(all_server_to_use.begin(), all_server_to_use.end(), server_desc);
    if(server_iter == all_server_to_use.end()) {
        return -2;
    }
    //remove the found server
    CCDLAPP_ << "Remove server "<< server_desc;
    all_server_to_use.erase(server_iter);
    return 0;
}
