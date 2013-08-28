/*	
 *	Configurable.h
 *	!CHOAS
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
#ifndef ChaosFramework_CConfigurable_h
#define ChaosFramework_CConfigurable_h

#include <boost/shared_ptr.hpp>

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    
    /**
     Define a rpotocol for update configuration in a class
     */
    class Configurable {
        
    public:
        virtual ~Configurable(){};
        virtual common::data::CDataWrapper* updateConfiguration(common::data::CDataWrapper*) = 0;
    };
}
#endif
