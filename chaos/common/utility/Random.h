/*
 * Copyright 2012, 27/08/2018 INFN
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

#ifndef __CHAOSFramework__Utility_Random_h
#define __CHAOSFramework__Utility_Random_h
#include <boost/atomic.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/random_device.hpp>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace common {
        namespace utility {
            
            /**
             Class for management fo random number
             */
            class Random {
                static boost::random_device rd;
                boost::random::uniform_int_distribution<std::int64_t> rnd_gen_int64;
            public:
                Random();
                explicit Random(std::int64_t min, std::int64_t max);
                ~Random();
                
                std::int64_t rand() const;
                operator std::int64_t() const;
            };
        }
    }
}

#endif /* __CHAOSFramework__Utility/Random_h */
