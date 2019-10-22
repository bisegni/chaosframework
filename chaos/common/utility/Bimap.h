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

#ifndef __CHAOSFramework__Bimap_h
#define __CHAOSFramework__Bimap_h

#include <chaos/common/global.h>

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
                
                size_t size() {
                    CHAOS_ASSERT(left_map.size() == right_map.size());
                    return left_map.size();
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__Bimap_h */
