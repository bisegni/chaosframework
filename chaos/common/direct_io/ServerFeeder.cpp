//
//  ServerFeeder.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <algorithm>

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/ServerFeeder.h>

using namespace chaos::common::direct_io;

typedef boost::unique_lock<boost::shared_mutex>	WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;

ServerFeeder::ServerFeeder() {
    
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

void ServerFeeder::addServer(std::string server_ip_port) {
    WriteLock	Lock(mutex_server_manipolation);
    //generate server hash
    uint32_t s_hash = data::cache::FastHash::hash(server_ip_port.c_str(), server_ip_port.size(), 0);
    
    //push hash into vec sequence
    vec_enable_hash_sequence.push_back(s_hash);
    
    //pu has and address into map
    map_server_address.insert(make_pair(s_hash, server_ip_port));
}

void ServerFeeder::removeServer(std::string server_ip_port) {
    WriteLock	Lock(mutex_server_manipolation);
    uint32_t s_hash = data::cache::FastHash::hash(server_ip_port.c_str(), server_ip_port.size(), 0);
    removeHashFromVector(vec_enable_hash_sequence, s_hash);
    removeHashFromVector(vec_disable_hash_sequence, s_hash);
    map_server_address.erase(s_hash);
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

void  ServerFeeder::getNextOnline(uint32_t& server_hash, std::string& server_ip_port) {
    ReadLock Lock(mutex_server_manipolation);
    current_online_index = (current_online_index + 1) % vec_enable_hash_sequence.size();
    return getCurrentOnline(server_hash, server_ip_port);
}

void ServerFeeder::getCurrentOnline(uint32_t& server_hash, std::string& server_ip_port) {
    ReadLock Lock(mutex_server_manipolation);
    server_hash = vec_enable_hash_sequence[current_online_index];
    server_ip_port = map_server_address[server_hash];
}

void ServerFeeder::getNextOffline(uint32_t& server_hash, std::string& server_ip_port) {
    ReadLock Lock(mutex_server_manipolation);
    current_offline_index = (current_offline_index + 1) % vec_disable_hash_sequence.size();
    getCurrentOffline(server_hash, server_ip_port);
}

void ServerFeeder::getCurrentOffline(uint32_t& server_hash, std::string& server_ip_port) {
    ReadLock Lock(mutex_server_manipolation);
    server_hash = vec_disable_hash_sequence[current_offline_index];
    server_ip_port = map_server_address[server_hash];
}

void ServerFeeder::getAllOnlineServer(std::vector<std::string>& server_list) {
    server_list.clear();
    for (std::vector<uint32_t>::iterator i = vec_enable_hash_sequence.begin();
         i != vec_enable_hash_sequence.end();
         i++) {
        server_list.push_back(map_server_address[*i]);
    }
}