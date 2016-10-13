/*
 *	URLServiceFeeder.h
 *	!CHAOS
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
#include <chaos/common/utility/Bimap.h>
#include <chaos/common/utility/NamedService.h>

#include <boost/thread.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>

namespace chaos {
    namespace common {
        namespace network {

            //!bidirectional map for url and index
            typedef chaos::common::utility::Bimap<std::string, uint32_t> URLIndexBimap;
            
            class URLServiceFeeder;

            class URLServiceFeederHandler {
            protected:
                friend class URLServiceFeeder;
                virtual void  disposeService(void *service_ptr) = 0;
                virtual void* serviceForURL(const URL& url,
                                            uint32_t service_index) = 0;
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
            class URLServiceFeeder:
            public utility::NamedService {
            public:
                    //!describe the feed mode for the fedder
                typedef enum URLServiceFeedMode {
                    URLServiceFeedModeFailOver,
                    URLServiceFeedModeRoundRobin
                } URLServiceFeedMode;


                    //! Default contructor
                /*!
                 Construct a new feeder with some option
                 \param alias the feeder alias
                 \param _handler the handler that wil manage some needed action of the feeder
                 */
                URLServiceFeeder(std::string alias,
                                 URLServiceFeederHandler *_handler);

                    //!Default destructor
                virtual ~URLServiceFeeder();

                    //!Add a new URL Object and return the associated index
                virtual uint32_t addURL(const URL& new_url, uint32_t priority = 0);

                    //!Remove an url with the index
                /*!
                 The remove operation can be perfored with
                 the deallocation of the service
                 \param idx index of the service
                 \param deallocate_service if true perform the service deallocation
                 */
                virtual void removeURL(uint32_t idx, bool dispose_service = false);

                //!return the url string from index
                std::string getURLForIndex(uint32_t idx);
                
                //!return the index from the url
                uint32_t getIndexFromURL(const std::string& url);
                
                /*!
                 Remove all url and service
                 */
                virtual void clear(bool dispose_service = true);

                /*!
                 Set the url as offline
                 */
                void setURLOffline(uint32_t idx);

                /*!
                 Set the url as online
                 */
                void setURLOnline(uint32_t idx);
                /*!
                 Check if is online
                 */
		
		bool isOnline(uint32_t idx);
                //! get the service to use according to the feeder node
                virtual void* getService(uint32_t idx);
                
                    //! get the service to use according to the feeder node
                virtual void* getService();

                    //! set the feed mode
                void setFeedMode(URLServiceFeedMode new_feed_mode);
            protected:
                boost::mutex mutex_internal;
                    //! contain the service index rule information
                struct URLService {
                    uint32_t index;
                    uint32_t priority;
                    bool online;
                    URL url;
                    void* service;
                    explicit URLService():
                    index(0),
                    priority(0),
                    online(false),
                    service(NULL){}
                    explicit URLService(uint32_t _index,
                                        uint32_t _priority,
                                        bool _online,
                                        const URL& _url,
                                        void *_service):
                    index(_index),
                    priority(_priority),
                    online(_online),
                    url(_url),
                    service(_service){}
                };

                    //! multi-index element service wrapper
                struct URLServiceIndex {
                    uint32_t global_index;
                    uint32_t priority;
                    uint64_t round_robing_start_position;
                    URLService *url_service;
                    explicit URLServiceIndex(uint32_t _global_index,
                                             uint32_t _priority,
                                             uint64_t _round_robing_start_position,
                                             URLService *_url_service):
                    global_index(_global_index),
                    priority(_priority),
                    round_robing_start_position(_round_robing_start_position),
                    url_service(_url_service){}

                      bool operator<(const URLServiceIndex& si)const{return global_index<si.global_index;}
                };

                    //multi-index class
                struct rb_pos_index{};
                struct priority_index{};
                struct position_index{};

                    //multi-index set
                typedef boost::multi_index_container<
                URLServiceIndex,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::identity<URLServiceIndex> >, //use operator <
                boost::multi_index::ordered_unique<boost::multi_index::tag<rb_pos_index>,  BOOST_MULTI_INDEX_MEMBER(URLServiceIndex, uint64_t, round_robing_start_position)>,
                boost::multi_index::ordered_unique<boost::multi_index::tag<position_index>,  BOOST_MULTI_INDEX_MEMBER(URLServiceIndex, uint32_t, global_index)>,
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<priority_index>, BOOST_MULTI_INDEX_MEMBER(URLServiceIndex, uint32_t, priority)>
                >
                > SetUrlServiceType;

                    //!round robin position index and iterator
                typedef boost::multi_index::index<SetUrlServiceType, rb_pos_index>::type                        SetUrlRBPositionIndex;
                typedef boost::multi_index::index<SetUrlServiceType, rb_pos_index>::type::iterator              SetUrlRBPositionIndexIterator;
                typedef boost::multi_index::index<SetUrlServiceType, rb_pos_index>::type::reverse_iterator      SetUrlRBPositionIndexReverseIterator;

                    //!priority index and iterator
                typedef boost::multi_index::index<SetUrlServiceType, priority_index>::type                      SetUrlPriorityIndex;
                typedef boost::multi_index::index<SetUrlServiceType, priority_index>::type::iterator            SetUrlPriorityIndexIterator;
                typedef boost::multi_index::index<SetUrlServiceType, priority_index>::type::reverse_iterator    SetUrlPriorityIndexReverseIterator;

                    //!priority index and iterator
                typedef boost::multi_index::index<SetUrlServiceType, position_index>::type                       SetUrlPositionIndex;
                typedef boost::multi_index::index<SetUrlServiceType, position_index>::type::iterator             SetUrlPositionIndexIterator;
                typedef boost::multi_index::index<SetUrlServiceType, position_index>::type::reverse_iterator     SetUrlPositionReverseIterator;

                    //!multi-index modeule to update the round robin position
                struct URLServiceIndexUpdateRBPosition
                {
                    URLServiceIndexUpdateRBPosition(uint64_t _new_pos):
                    new_pos(_new_pos){}

                    void operator()(URLServiceIndex& si) {
                        si.round_robing_start_position=new_pos;
                    }

                private:
                    uint64_t new_pos;
                };

                    //round robin last position
                uint64_t last_round_robin_index;

                uint32_t list_size;

                    //point to current service
                URLService *current_service;

                    //contains all the service
                URLService **service_list;
                
                //map to correlate url stirng to index
                URLIndexBimap mapping_url_index;
                
                    //multi-index structure for managment of online service
                SetUrlServiceType   set_urls_online;
                SetUrlPositionIndex&    set_urls_pos_index;
                SetUrlRBPositionIndex&  set_urls_rb_pos_index;

                std::set<uint32_t> offline_urls;
                std::set<uint32_t> available_url;

                URLServiceFeederHandler *handler;
                URLServiceFeedMode feed_mode;

                inline void grow();
                inline URLService* getNext();
                inline void removeFromOnlineQueue(uint32_t url_index);
                
                    //!work on sequence incremneting for every fetched index the sequence by on(in this way is achieved the wroud robin
                URLService *getNextFromSetByRoundRobin();
                URLService *getNextFromSetByPriority();
            };
            
        }
    }
}
#endif /* defined(__CHAOSFramework__URLServiceFeeder__) */
