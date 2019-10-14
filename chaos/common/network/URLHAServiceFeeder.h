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

#ifndef __CHAOSFramework__URLHAServiceFeeder_h
#define __CHAOSFramework__URLHAServiceFeeder_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/network/URLServiceFeeder.h>

#include <queue>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace network {
            
            //! handler for check the online status of the service
            class URLHAServiceCheckerFeederHandler {
            public:
                virtual bool serviceOnlineCheck(void *service_ptr) = 0;
            };
            
            //!contains the infromation for retry logic
            struct ServiceRetryInformation {
                const uint32_t offline_index;
                const std::string offline_url;
                uint32_t retry_times;
                uint64_t retry_timeout;
                
                ServiceRetryInformation(const uint32_t _offline_index,
                                        const std::string& _offline_url):
                offline_index(_offline_index),
                offline_url(_offline_url),
                retry_times(0),
                retry_timeout(0){}
                
                ServiceRetryInformation(const ServiceRetryInformation& sri):
                offline_index(sri.offline_index),
                offline_url(sri.offline_url),
                retry_times(sri.retry_times),
                retry_timeout(sri.retry_timeout){}
            };
            
            //! High availability service feeder
            /*!
             This class extend URLServiceFeeder to realize an HA behaviour
             on the url that are contained into the base class
             */
            class URLHAServiceFeeder:
            protected URLServiceFeeder {
                uint32_t min_retry_time;
                uint32_t max_retry_time;
                URLHAServiceCheckerFeederHandler *service_checker_handler;
                
                boost::mutex mutex_queue;
                std::queue< ChaosSharedPtr<ServiceRetryInformation> > retry_queue;
                std::queue<uint32_t> respawned_queue;
            public:
                
                using URLServiceFeeder::hasURL;
                using URLServiceFeeder::getIndexFromURL;
                using URLServiceFeeder::isOnline;
                URLHAServiceFeeder(std::string alias,
                                   URLServiceFeederHandler *_service_feeder_handler,
                                   URLHAServiceCheckerFeederHandler *_service_checker_handler);
                ~URLHAServiceFeeder();
                
                //! Set the retry time edge
                void setRetryTime(const uint32_t _min_retry_time,
                                  const uint32_t _max_retry_time);
                //!Add a new URL Object and return the associated index
                uint32_t addURL(const URL& new_url, uint32_t priority = 0);
                
                //!Remove an url with the index
                /*!
                 The remove operation can be perfored with
                 the deallocation of the service
                 \param idx url_to_remove of the service
                 \param deallocate_service if true perform the service deallocation
                 */
                void removeURL(const URL& url_to_remove,
                               bool dispose_service = false);
                
                /*!
                 Remove all url and service
                 */
                void clear(bool dispose_service = true);
                
                //! get the service to use according to the feeder node
                void* getService();
                
                //! set url has offline
                void setURLAsOffline(const std::string& remote_address);
                
                //! set url has offline
                void setIndexAsOffline(const uint32_t remote_index);
                
                //!check if a service has been respawned
                /*!
                 this method is thread safe and it must be called from another tread because it trye to see
                 if the service is respawn and handler could be try to comunicate with it.
                 So the handler work mode will determinate if this method can be called, or couldn't, in a separate thread.
                 */
                void checkForAliveService();
                
                size_t getOfflineSize();
            };
            
        }
    }
}

#endif /* __CHAOSFramework__URLHAServiceFeeder_h */
