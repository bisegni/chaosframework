/*
 *	TemplatedKeyObjectContainer.h
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

#ifndef CHAOSFramework_KeyObjectContainer_h
#define CHAOSFramework_KeyObjectContainer_h

#include <chaos/common/global.h>
#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
            template<typename T>
            void dummy_free(T e) {}
            
            template<typename K, typename O >
            class TemplatedKeyObjectContainer {
                boost::shared_mutex mutex_organizer_map;
                
            public:
                typedef typename std::map< K, O >::iterator     ContainerOrganizerIterator;
                
            protected:
                std::map< K, O > organizer_map;
                
            protected:
                virtual void freeObject(K key, O element) {}
                
            public:
                TemplatedKeyObjectContainer(){};
                ~TemplatedKeyObjectContainer(){clearElement();};
                
                void clearElement() {
                    for(ContainerOrganizerIterator iter = organizer_map.begin();
                        iter != organizer_map.end();
                        iter++) {
                        freeObject(iter->first, iter->second);
                    }
                    organizer_map.clear();
                }
                
                // allocate a new channel by the instancer
                void registerElement(K key, O item) {
                    boost::unique_lock<boost::shared_mutex>	Lock(mutex_organizer_map);
                    
                    //associate the instance
                    organizer_map.insert(std::make_pair(key, item));
                }
                
                // dispose the channel instance
                void deregisterElementKey(K key) {
                    try{
                        boost::unique_lock<boost::shared_mutex>	Lock(mutex_organizer_map);
                        organizer_map.erase(key);
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::lock_error> >& lock_exception) {
                        LERR_ << lock_exception.what();
                    }
                }
                
                inline bool hasKey(K key) {
                    return organizer_map.count(key) > 0;
                }
                
                inline O accessItem(K key) {
                    try{
                        boost::shared_lock<boost::shared_mutex> lock(mutex_organizer_map);
                        if(organizer_map.count(key))
                            return organizer_map[key];
                        else
                            return NULL;
                    }catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::lock_error> >& lock_exception) {
                        LERR_ << lock_exception.what();
                        return NULL;
                    }
                }
                
            };
        }
    }
}

#endif
