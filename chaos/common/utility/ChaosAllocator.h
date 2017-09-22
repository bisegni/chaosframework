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
