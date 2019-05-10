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
                
                typedef struct TKOCElement {
                    const K& key;
                    O element;
                }TKOCElement;
                
                class FreeHandler {
                public:
                    virtual void freeObject(const TKOCElement& elemet_to_delete) = 0;
                };
                
            protected:
                std::map< K, O > organizer_map;
                
            protected:
                FreeHandler *free_handler;
                
            public:
                TemplatedKeyObjectContainer(FreeHandler *_free_handler):
                free_handler(_free_handler){};
                
                virtual ~TemplatedKeyObjectContainer(){
                    clearElement();
                };
                
                void clearElement() {
                    ContainerOrganizerIterator it = organizer_map.begin();
                    ContainerOrganizerIterator end = organizer_map.end();
                    while(it != end) {
                        ContainerOrganizerIterator tmp = it++;
                        if(free_handler) {
                            TKOCElement e = {tmp->first, tmp->second};
                            free_handler->freeObject(e);
                        }
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
