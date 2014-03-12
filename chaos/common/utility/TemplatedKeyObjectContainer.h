//
//  TemplatedKeyObjectContainer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 09/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_KeyObjectContainer_h
#define CHAOSFramework_KeyObjectContainer_h

#include <boost/thread.hpp>

namespace chaos {
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
                organizer_map.insert(make_pair(key, item));
            }
            
            // dispose the channel instance
            void deregisterElementKey(K key) {
                boost::unique_lock<boost::shared_mutex>	Lock(mutex_organizer_map);
                organizer_map.erase(key);
            }

			inline bool hasKey(K key) {
				return organizer_map.count(key) > 0;
			}
			
            inline O accessItem(K key) {
				boost::shared_lock<boost::shared_mutex> lock(mutex_organizer_map);
				if(organizer_map.count(key))
				return organizer_map[key];
            }
            
        };
        
    }
}

#endif
