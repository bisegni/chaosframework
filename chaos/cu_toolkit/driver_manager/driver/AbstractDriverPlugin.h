/*
 *	AbstractDriverPlugin.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef CHAOSFramework_AbstractDriverPlugin_h
#define CHAOSFramework_AbstractDriverPlugin_h

#include <chaos/common/plugin/AbstractPlugin.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>

namespace chaos{
    namespace cu {
        namespace dm {
            namespace driver {
                
#define REGISTER_CU_DRIVER_CLASS(a, v, n)\
class n;\
ALLOCATOR(a,v,n)\
class n : protected AbstractDriverPlugin
                
                //! Plugin abstraction for the control unit driver
                /*!
                 
                 */
                class AbstractDriverPlugin: public chaos::common::plugin::AbstractPlugin {
                    AbstractDriver *driverReference;
                    
                    AbstractDriverPlugin(AbstractDriver *_driverReference);
                    ~ AbstractDriverPlugin();
                };
                
            }
        }
    }
}

#endif
