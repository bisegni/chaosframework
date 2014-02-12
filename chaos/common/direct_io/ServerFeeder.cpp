/*
 *	ServerFeeder.cpp
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
#include <algorithm>

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/ServerFeeder.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex DirectIOHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex DirectIOIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}");

using namespace chaos::common::direct_io;

typedef boost::unique_lock<boost::shared_mutex>	WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;

ServerFeeder::ServerFeeder():current_online_index(-1), current_offline_index(-1){
    
}

ServerFeeder::~ServerFeeder(){
    clear();
}

void ServerFeeder::removeHashFromVector(std::vector<uint32_t>& hash_vec, uint32_t server_hash) {
    ServerHashVectorIterator i = std::find(hash_vec.begin(), hash_vec.end(), server_hash);
    if(i != hash_vec.end()) {
        hash_vec.erase(i);
    }
}
void ServerFeeder::addHashToVector(std::vector<uint32_t>& hash_vec, uint32_t server_hash) {
    ServerHashVectorIterator i = std::find(hash_vec.begin(), hash_vec.end(), server_hash);
    if(i != hash_vec.end()) {
        hash_vec.push_back(server_hash);
    }
}

void ServerFeeder::decoupleServerDescription(std::string server_desc, std::vector<std::string>& servers_desc) {
	std::vector<std::string> server_desc_tokens;
	boost::algorithm::split(server_desc_tokens, server_desc, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	
	//create the two servers description
	servers_desc.push_back(boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[1]));
	servers_desc.push_back(boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[2]));
}

bool ServerFeeder::addServer(std::string server_desc) {
    WriteLock	Lock(mutex_server_manipolation);
	
	boost::algorithm::trim(server_desc);
	std::string normalized_server_desc = boost::algorithm::to_lower_copy(server_desc);
	
	//check if the description is well formed
	if(!regex_match(server_desc, DirectIOHostNameRegExp) && !regex_match(server_desc, DirectIOIPAndPortRegExp)) return false;
	
    //generate server hash
    uint32_t s_hash = data::cache::FastHash::hash(normalized_server_desc.c_str(), normalized_server_desc.size(), 0);
    
    //push hash into vec sequence
    vec_enable_hash_sequence.push_back(s_hash);
    
    //pu has and address into map
	std::vector< std::string > servers_decopled;
	decoupleServerDescription(normalized_server_desc, servers_decopled);
    map_server_address.insert(make_pair(s_hash, servers_decopled));
	
	if(current_online_index==-1) current_online_index = 0;
	return true;
}

void ServerFeeder::removeServer(uint32_t server_hash) {
    WriteLock	Lock(mutex_server_manipolation);
    removeHashFromVector(vec_enable_hash_sequence, server_hash);
    removeHashFromVector(vec_disable_hash_sequence, server_hash);
    map_server_address.erase(server_hash);
	if(map_server_address.size()==0) current_online_index = -1;
}

void ServerFeeder::putOfflineServer(uint32_t server_hash) {
    WriteLock	Lock(mutex_server_manipolation);
    removeHashFromVector(vec_enable_hash_sequence, server_hash);
    addHashToVector(vec_disable_hash_sequence, server_hash);
}

void ServerFeeder::putOnlineServer(uint32_t server_hash) {
    WriteLock	Lock(mutex_server_manipolation);
    removeHashFromVector(vec_disable_hash_sequence, server_hash);
    addHashToVector(vec_enable_hash_sequence, server_hash);
}

void ServerFeeder::clear() {
    WriteLock	Lock(mutex_server_manipolation);
    vec_enable_hash_sequence.clear();
    vec_disable_hash_sequence.clear();
    map_server_address.clear();
}

bool ServerFeeder::getNextOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc) {
    ReadLock Lock(mutex_server_manipolation);
    current_online_index = (current_online_index + 1) % vec_enable_hash_sequence.size();
    return getCurrentOnline(server_hash, priority_server_desc, service_server_desc);
}

bool ServerFeeder::getCurrentOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc) {
    ReadLock Lock(mutex_server_manipolation);
    server_hash = vec_enable_hash_sequence[current_online_index];
    priority_server_desc = map_server_address[server_hash][0];
	service_server_desc = map_server_address[server_hash][1];
}

bool ServerFeeder::getNextOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc) {
    ReadLock Lock(mutex_server_manipolation);
    current_offline_index = (current_offline_index + 1) % vec_disable_hash_sequence.size();
    getCurrentOffline(server_hash, priority_server_desc, service_server_desc);
}

bool ServerFeeder::getCurrentOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc) {
    ReadLock Lock(mutex_server_manipolation);
    server_hash = vec_disable_hash_sequence[current_offline_index];
    priority_server_desc = map_server_address[server_hash][0];
	service_server_desc = map_server_address[server_hash][1];
}

bool ServerFeeder::getAllOnlineServer(std::vector< std::vector<std::string> >& server_list) {
    server_list.clear();
    for (std::vector<uint32_t>::iterator i = vec_enable_hash_sequence.begin();
         i != vec_enable_hash_sequence.end();
         i++) {
        server_list.push_back(map_server_address[*i]);
    }
}