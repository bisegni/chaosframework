/*
 *	InstrumentEventDescriptor.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 19/08/12.
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

/*
 Intellectual property of   Copyright (c) 2012 INFN. All rights reserved.
 */

#ifndef __CHAOSFramework__InstrumentEventDescriptor__
#define __CHAOSFramework__InstrumentEventDescriptor__
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace event{
        namespace instrument {
                //REGISTER_AND_DEFINE_NOALIAS_DERIVED_CLASS_FACTORY(InstrumentEventDescriptor, EventDescriptor ) {
            class InstrumentEventDescriptor : public EventDescriptor {
            public:
                
            };

        }
    }
    
}
#endif /* defined(__CHAOSFramework__InstrumentEventDescriptor__) */
