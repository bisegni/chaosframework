/*
 *	ServerFeeder.h
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
                int32_t current_online_index;
                
                
                //! point to the current disable hash index
                int32_t current_offline_index;
                
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
                bool getNextOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                bool getCurrentOnline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                bool getNextOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                bool getCurrentOffline(uint32_t& server_hash, std::string& priority_server_desc, std::string& service_server_desc);
                bool getAllOnlineServer(std::vector< std::vector<std::string> >& server_list);
                
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ServerFeeder__) */
