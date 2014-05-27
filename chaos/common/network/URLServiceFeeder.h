/*
 *	URLServiceFeeder.h
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

#ifndef __CHAOSFramework__URLServiceFeeder__
#define __CHAOSFramework__URLServiceFeeder__

#include <set>
#include <string>

#include <queue>

#include <chaos/common/network/URL.h>
#include <chaos/common/utility/NamedService.h>
namespace chaos {
    namespace common {
        namespace network {
            
            class URLServiceFeeder;
            
            class URLServiceFeederHandler {
            protected:
                friend class URLServiceFeeder;
                virtual void  disposeService(void *service_ptr) = 0;
                virtual void* serviceForURL(URL url) = 0;
            };
            
                //! Service feeder helper class
            /*!
             This class help to associate a remote service ti an url, and help
             to manage the online and offline state managment of the service.
             Two queue are used to separate the index of valid and ofline url.
             when next url is requested, according roundrobin or fail over behaviour
             the current or next id on 'online_urls' queue is returned. In roundrobin
             mode the top element is removed from the queue and insert to the end.
             NOTE: the service pointer into URLService structure is not deallocated.
             */
            class URLServiceFeeder: public chaos::NamedService {
            public:
                    //!describe the feed mode for the fedder
                typedef enum URLServiceFeedMode {
                    URLServiceFeedModeFailOver,
                    URLServiceFeedModeRoundRobin
                } URLServiceFeedMode;
                
                struct URLService {
                    uint32_t index;
                    uint32_t priority;
                    bool online;
                    URL url;
                    void* service;
                    bool operator < (const URLService& b) const {
                        return priority < b.priority;
                    }
                    bool operator < (const URLService*& b) const {
                        return priority < b->priority;
                    }
                };
                
                    //! Default contructor
                /*!
                 Construct a new feeder with some option
                 \param alias the feeder alias
                 \param _handler the handler that wil manage some needed action of the feeder
                 */
                URLServiceFeeder(std::string alias, URLServiceFeederHandler *_handler);
                
                    //!Default destructor
                virtual ~URLServiceFeeder();
                
                    //!Add a new URL Object and return the associated index
                uint32_t addURL(URL new_url, uint32_t priority = 0);
                
                    //!get eh service with the relative index
                void* getService(uint32_t idx);
                
                    //!Remove a service
                /*!
                 The remove operation can be perfored with
                 the deallocation of the service
                 \param idx index of the service
                 \param deallocate_service if true perform the service deallocation
                 */
                void removeURL(uint32_t idx, bool deallocate_service = false);
                
                /*!
                 Set the url as offline
                 */
                void setURLOffline(uint32_t idx);
                
                /*!
                 Set the url as online
                 */
                void setURLOnline(uint32_t idx);
                
                    //! get the service to use according to the feeder node
                void* getService();
                
                    //! set the feed mode
                void setFeedMode(URLServiceFeedMode new_feed_mode);
            protected:
                uint32_t list_size;
                
                URLService *current_service;
                URLService **service_list;
                
                std::priority_queue< URLService*, std::vector< URLService* >, less<vector<URLService*>::value_type> > online_urls;
                
                std::set<uint32_t> offline_urls;
                std::set<uint32_t> available_url;

                URLServiceFeederHandler *handler;
                URLServiceFeedMode feed_mode;
                
                inline void grow();
                inline URLService* getNext();
            };
            
        }
    }
}
#endif /* defined(__CHAOSFramework__URLServiceFeeder__) */
