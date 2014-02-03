//
//  ServerFeeder.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ServerFeeder__
#define __CHAOSFramework__ServerFeeder__

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

#include <boost/thread.hpp>
namespace chaos {
	namespace common {
		namespace direct_io {
            
            //!Utility class for the managment of the server sequence for direct io client
            class ServerFeeder {
                
                //! point to the current enable hash index
                uint32_t current_online_index;
                
                
                //! point to the current disable hash index
                uint32_t current_offline_index;
                
                boost::shared_mutex mutex_server_manipolation;
                
                typedef std::vector<uint32_t>::iterator ServerHashVectorIterator;
                
                //! order of usage the server hashes(the enabled server)
                std::vector<uint32_t>              vec_enable_hash_sequence;

                //! contain the hash of offline server, order by the last offline found
                std::vector<uint32_t>              vec_disable_hash_sequence;
                
                //! server hashes and address map
                std::map<uint32_t, std::vector<std::string> >    map_server_address;
                
                inline void removeHashFromVector(std::vector<uint32_t>& hash_vec, uint32_t server_hash);
                inline void addHashToVector(std::vector<uint32_t>& hash_vec, uint32_t server_hash);
				inline void decoupleServerDescription(std::string server_desc,  std::vector<std::string>& servers_desc);
            public:
                ServerFeeder();
                ~ServerFeeder();
                bool addServer(std::string server_desc);
                void removeServer(uint32_t server_hash);
                void putOfflineServer(uint32_t server_hash);
                void putOnlineServer(uint32_t server_hash);
                void clear();
                void getNextOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                inline void getCurrentOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                void getNextOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                inline void getCurrentOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                void getAllOnlineServer(std::vector< std::vector<std::string> >& server_list);
                
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ServerFeeder__) */
