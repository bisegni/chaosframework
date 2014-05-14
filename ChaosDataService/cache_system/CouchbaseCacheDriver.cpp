/*
 *	CouchbaseChacheDriver.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "CouchbaseCacheDriver.h"

#include <string>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#define CouchbaseCacheDriver_LOG_HEAD "[CouchbaseCacheDriver] - "

#define CCDLAPP_ LAPP_ << CouchbaseCacheDriver_LOG_HEAD
#define CCDLDBG_ LDBG_ << CouchbaseCacheDriver_LOG_HEAD << __FUNCTION__ << " - "
#define CCDLERR_ LERR_ << CouchbaseCacheDriver_LOG_HEAD

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex CouchbaseHostNameOnlyRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+");
//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex CouchbaseHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex CouchbaseIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}");


using namespace chaos::data_service::cache_system;

void CouchbaseCacheDriver::errorCallback(lcb_t instance,
										  lcb_error_t err,
										  const char *errinfo) {
    (void)instance;
	((CouchbaseCacheDriver*)lcb_get_cookie(instance))->last_err = err;
	if(errinfo) {
		CCDLERR_<< "Error " << err << " with message " << errinfo;
		((CouchbaseCacheDriver*)lcb_get_cookie(instance))->last_err_str = errinfo;
	} else {
		((CouchbaseCacheDriver*)lcb_get_cookie(instance))->last_err_str = "";
	}
}

void CouchbaseCacheDriver::getCallback(lcb_t instance,
									   const void *cookie,
									   lcb_error_t error,
									   const lcb_get_resp_t *resp) {
	(void)instance;
	ResultValue *result = new ResultValue();
	if(error == LCB_SUCCESS) {
		result->value_len = (uint32_t)resp->v.v0.nbytes;
		result->value = std::malloc(resp->v.v0.nbytes);
		std::memcpy(result->value, resp->v.v0.bytes, resp->v.v0.nbytes);

	}
	result->err = error;
	((CouchbaseCacheDriver*)cookie)->addAnswer(result);
}

void CouchbaseCacheDriver::setCallback(lcb_t instance,
										   const void *cookie,
										   lcb_storage_t operation,
										   lcb_error_t error,
										   const lcb_store_resp_t *resp) {
	(void)instance;
}

CouchbaseCacheDriver::CouchbaseCacheDriver(std::string alias):
CacheDriver(alias),
instance(NULL),result_queue(1) {
	lcb_uint32_t ver;
	const char *msg = lcb_get_version(&ver);
	CCDLAPP_ << "Couchbase sdk version: " << msg;
}

CouchbaseCacheDriver::~CouchbaseCacheDriver() {
	if(instance) lcb_destroy(instance);
}

int CouchbaseCacheDriver::putData(void *element_key, uint8_t element_key_len,  void *value, uint32_t value_len) {
	//boost::shared_lock<boost::shared_mutex> lock(mutex_server);
	lcb_store_cmd_t cmd;
    const lcb_store_cmd_t * const commands[] = { &cmd };
	
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = element_key;
    cmd.v.v0.nkey = element_key_len;
    cmd.v.v0.bytes = value;
    cmd.v.v0.nbytes = value_len;
    cmd.v.v0.operation = LCB_SET;
    if ((last_err = lcb_store(instance, this, 1, commands)) != LCB_SUCCESS) {
		CCDLERR_<< "Fail to set value -> "<< lcb_strerror(NULL, last_err);
		return -1;
    }
	lcb_wait(instance);
	if(last_err != LCB_SUCCESS) {
		CCDLERR_<< "Fail to set value with last_err "<< last_err << " with message " << last_err_str;
		last_err = LCB_SUCCESS;
		return -1;
	}
	return 0;
}

int CouchbaseCacheDriver::getData(void *element_key, uint8_t element_key_len,  void **value, uint32_t& value_len) {
	//boost::shared_lock<boost::shared_mutex> lock(mutex_server);
	lcb_get_cmd_t cmd;
	lcb_error_t err = LCB_SUCCESS;
	const lcb_get_cmd_t *commands[1];
	
	commands[0] = &cmd;
	memset(&cmd, 0, sizeof(cmd));
	cmd.v.v0.key = element_key;
	cmd.v.v0.nkey = element_key_len;
	
	err = lcb_get(instance, (void*)this, 1, commands);
	if (err != LCB_SUCCESS) {
		CCDLERR_<< "Fail to get value -> "<< lcb_strerror(NULL, err);
		return -1;
	}
	lcb_wait(instance);
	ResultValue *result = NULL;
	if(result_queue.pop(result)){
		*value = (void*)result->value;
		value_len = result->value_len;
		delete result;
	}
	if(last_err != LCB_SUCCESS) {
		CCDLERR_<< "Fail to set value with last_err "<< last_err << " with message " << last_err_str;
		last_err = LCB_SUCCESS;
		return -1;
	}
	return 0;
}

void CouchbaseCacheDriver::addAnswer(ResultValue *got_value) {
	result_queue.push(got_value);
}

bool CouchbaseCacheDriver::validateString(std::string& server_description) {
	boost::algorithm::trim(server_description);
	std::string normalized_server_desc = boost::algorithm::to_lower_copy(server_description);
	
	//check if the description is well formed
	if(!regex_match(normalized_server_desc, CouchbaseHostNameOnlyRegExp) &&
	   !regex_match(normalized_server_desc, CouchbaseHostNameRegExp) &&
	   !regex_match(normalized_server_desc, CouchbaseIPAndPortRegExp)) return false;
	return true;
}

int CouchbaseCacheDriver::addServer(std::string server_desc) {
	if(!validateString(server_desc)) return -1;
	
	ServerIterator server_iter = std::find(all_server_to_use.begin(), all_server_to_use.end(), server_desc);
	if(server_iter != all_server_to_use.end()) {
		return -2;
	}
	//add new server
	CCDLAPP_ << "Add server "<< server_desc;
	all_server_to_use.push_back(server_desc);
	return 0;
}

int CouchbaseCacheDriver::removeServer(std::string server_desc) {
	if(!validateString(server_desc)) return -1;
	ServerIterator server_iter = std::find(all_server_to_use.begin(), all_server_to_use.end(), server_desc);
	if(server_iter == all_server_to_use.end()) {
		return -2;
	}
	//remove the found server
	CCDLAPP_ << "Remove server "<< server_desc;
	all_server_to_use.erase(server_iter);
	return 0;
}

int CouchbaseCacheDriver::updateConfig() {
	//lock for other access
	//boost::unique_lock<boost::shared_mutex> lock(mutex_server);
	CCDLAPP_ << "Conenct to all setupped server";
	//destroy the instance in case we have used it
	if(instance) {
		CCDLAPP_ << "Destroy last session";
		lcb_destroy(instance);
	}
	//clear the configuration
	memset(&create_options, 0, sizeof(create_options));
	
	lcb_config_transport_t transports[] = {
		LCB_CONFIG_TRANSPORT_CCCP,
		LCB_CONFIG_TRANSPORT_LIST_END
	};
	
	//create_options
	create_options.version = 2;
	create_options.v.v2.transports = transports;
	create_options.v.v2.user = "chaos";
	create_options.v.v2.passwd = "chaos";
	create_options.v.v2.bucket = "chaos";
	all_server_str.assign("");
	for (ServerIterator iter = all_server_to_use.begin();
		 iter!=all_server_to_use.end();
		 iter++) {
		all_server_str.append(*iter);
		all_server_str.append(";");
	}
	if(all_server_str.size()) {
		//remove the last ';' character
		all_server_str.resize(all_server_str.size()-1);
	}
	
	CCDLAPP_ << "Create new session";
	//assign the host string to the configuration
	create_options.v.v2.host = all_server_str.c_str();
	
	//create the instance
	last_err = lcb_create(&instance, &create_options);
    if (last_err != LCB_SUCCESS) {
		CCDLERR_<< "Error initializing the session -> "<< lcb_strerror(NULL, last_err);
        return -1;
    }
	
	lcb_set_cookie(instance, this);
	
	lcb_behavior_set_syncmode(instance, LCB_ASYNCHRONOUS);

	/* Set up the handler to catch all errors! */
	lcb_set_error_callback(instance, CouchbaseCacheDriver::errorCallback);

	/* initiate the connect sequence in libcouchbase */
    last_err = lcb_connect(instance);
    if (last_err != LCB_SUCCESS) {
        CCDLERR_<< "Error connecting the session -> " << lcb_strerror(NULL, last_err);
        return -1;
    }
	
	/* run the event loop and wait until we've connected */
    lcb_wait(instance);
	
	if(last_err == LCB_SUCCESS) {
		/* set up a callback for our get and set requests  */
		lcb_set_get_callback(instance, CouchbaseCacheDriver::getCallback);
		lcb_set_store_callback(instance, CouchbaseCacheDriver::setCallback);
	}
	lcb_size_t num_events = 0;
	lcb_cntl(instance, LCB_CNTL_GET, LCB_CNTL_CONFERRTHRESH, &num_events);
	num_events = 1;
	lcb_cntl(instance, LCB_CNTL_SET, LCB_CNTL_CONFERRTHRESH, &num_events);

	lcb_cntl(instance, LCB_CNTL_GET, LCB_CNTL_CONFDELAY_THRESH, &num_events);
	num_events = 500000;
	lcb_cntl(instance, LCB_CNTL_SET, LCB_CNTL_CONFDELAY_THRESH, &num_events);

	return !(last_err == LCB_SUCCESS);
}