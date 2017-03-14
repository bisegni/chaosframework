/*
 *	ChaosAllocator.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__0F17F38_68E0_43D4_B84C_136D8BF488C0_ChaosAllocator_h
#define __CHAOSFramework__0F17F38_68E0_43D4_B84C_136D8BF488C0_ChaosAllocator_h

#include <stdlib.h>

namespace chaos {
    namespace common {
        namespace utility {
            
            class ChaosAllocator {
            public:
                void* malloc(size_t sz) { return std::malloc(sz);}
                void* realloc(void *p, size_t sz) { return std::realloc(p, sz);}
                void free(void *p) { std::free(p);}
            };
            
        }
    }
}

#endif /* __CHAOSFramework__0F17F38_68E0_43D4_B84C_136D8BF488C0_ChaosAllocator_h */
