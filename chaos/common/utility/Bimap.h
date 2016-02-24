/*
 *	Bimap.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 24/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__Bimap_h
#define __CHAOSFramework__Bimap_h

#include <chaos/common/data/cache/FastHash.h>

#include <map>

namespace chaos {
    namespace common {
        namespace utility {
            //! class that permit to sumulate a bidirectional map
            /*!
             poor bad implementation
             */
            template<typename A, typename B>
            class Bimap {
                std::map<A,B> left_map;
                std::map<B,A> right_map;
            public:
                void insert(A a, B b){
                    left_map.insert(make_pair(a, b));
                    right_map.insert(make_pair(b, a));
                }
                
                bool hasLeftKey(A a) {
                    return left_map.count(a) != 0;
                }
                
                bool hasRightKey(B b) {
                    return right_map.count(b) != 0;
                }
                
                void removebyLeftKey(A a) {
                    if(!hasLeftKey(a)) return;
                    right_map.erase(left_map[a]);
                    left_map.erase(a);
                }
                
                void removebyRightKey(B b) {
                    if(!hasRightKey(b)) return;
                    left_map.erase(right_map[b]);
                    right_map.erase(b);
                }
                
                B findByLeftKey(A a) {
                    return left_map[a];
                }
                
                A findByRightKey(B b) {
                    return right_map[b];
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__Bimap_h */
