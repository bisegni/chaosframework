/*
 *	DriverWrapperPlugin.h
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

#ifndef CHAOSFramework_DriverWrapperPlugin_h
#define CHAOSFramework_DriverWrapperPlugin_h

#include <chaos/common/plugin/PluginDefinition.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>

namespace chaos{
    namespace cu {
        namespace dm {
            namespace driver {
                
                class AbstractDriverPlugin: public chaos::common::plugin::AbstractPlugin {
                    AbstractDriver *abstractDriver;
                public:
                    AbstractDriverPlugin(AbstractDriver *_abstractDriver);
                    virtual ~AbstractDriverPlugin();
                    
                    
                    //! Proxy for create a new accessor to the driver
                    
                    DriverAccessor * const getNewAccessor();
                    
                    //! Proxy for dispose an accessor
                    void releaseAccessor(DriverAccessor *accessor);
                };
 
#define ADD_CU_DRIVER_PLUGIN_SUPERCLASS public AbstractDriver, public AbstractDriverPlugin
            
#define DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(t) t::t():chaos::cu::dm::driver::AbstractDriverPlugin(this)
                
                //! Define a plugin for a driver
                /*!
                 Define a class that implement the AbstractDriver as plugin
                 \param a alias used to instantiate the driver
                 \param v the version of the driver
                 \param n the class that implement the AbstractDriver
                 */
#define OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(a, v, n)\
OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a, v, CUDRV, n, AbstractDriverPlugin)
  

#define REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE REGISTER_PLUGIN_CLASS_INIT_ATTRIBUTE

                
#define CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION CLOSE_PLUGIN_CLASS_DEFINITION
            }
        }
    }
}

#endif
