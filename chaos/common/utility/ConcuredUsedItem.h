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

#ifndef CHAOSFramework_ConcuredUsedItem_h
#define CHAOSFramework_ConcuredUsedItem_h

#include <boost/atomic.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
            /*!
             Class for helping other class to mark how much are
             in use
             */
            class ConcuredUsedItem {
                boost::atomic<uint16_t> worker_on_item;
            public:
                inline void increasTheWorker(){worker_on_item++};
                inline void decreaseTheWorker(){worker_on_item++};
                inline uint16_t getWorkersNUmber(){return worker_on_item;}
            };
        }
    }
}

#endif
