//
//  MemcachedCacheDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "MemcachedCacheDriver.h"

#include <string>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex MemcachedHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex MemcachedIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}");


using namespace chaos::data_service::cache_system;
MemcachedCacheDriver::MemcachedCacheDriver(std::string alias): CacheDriver(alias), memcache_client(NULL) {
	
	//memcached_return_t configResult = MEMCACHED_SUCCESS;
	
	memcache_client = memcached(NULL, 0);
}

MemcachedCacheDriver::~MemcachedCacheDriver() {
	if(memcache_client) {
		memcached_free(memcache_client);
	}
}

int MemcachedCacheDriver::putData(void *element_key, uint8_t element_key_len,  void *value, uint32_t value_len) {
	memcached_return_t err = MEMCACHED_SUCCESS;
	err = memcached_set(memcache_client, (const char *)element_key, element_key_len, (const char*)value, (size_t)value_len, 0, 0);
	return err != MEMCACHED_SUCCESS;
}

int MemcachedCacheDriver::getData(void *element_key, uint8_t element_key_len,  void **value, uint32_t& value_len) {
	uint32_t flags = 0;
	size_t len;
	memcached_return_t mcSetResult = MEMCACHED_SUCCESS;
	
	*value =  memcached_get(memcache_client, (const char *)element_key, element_key_len, &len, &flags,  &mcSetResult);
	value_len = (uint32_t)len;
	return *value == NULL;
}

bool MemcachedCacheDriver::validateString(std::string& server_description, std::vector<std::string> &tokens) {
	boost::algorithm::trim(server_description);
	std::string normalized_server_desc = boost::algorithm::to_lower_copy(server_description);
	
	//check if the description is well formed
	if(!regex_match(normalized_server_desc, MemcachedHostNameRegExp) && !regex_match(normalized_server_desc, MemcachedIPAndPortRegExp)) return false;
	
	boost::algorithm::split(tokens, normalized_server_desc, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	return true;
}

int MemcachedCacheDriver::addServer(std::string server_desc) {
	//check if the description is well formed
	std::vector<std::string> tokens;
	memcached_return_t err = MEMCACHED_SUCCESS;
	if(!validateString(server_desc, tokens)) return -1;

	err = memcached_server_add(memcache_client, tokens[0].c_str(), boost::lexical_cast<int>(tokens[1]));
	return err != MEMCACHED_SUCCESS;
}

int MemcachedCacheDriver::removeServer(std::string server_desc) {
	//std::vector<std::string> tokens;
	memcached_return_t err = MEMCACHED_SUCCESS;
	//if(!validateString(server_desc, tokens)) return false;
	
	//err = memcached_server_remove(memcache_client, tokens[0].c_str(), boost::lexical_cast<int>(tokens[1]));
	return err != MEMCACHED_SUCCESS;
}

int MemcachedCacheDriver::updateConfig() {
	
}