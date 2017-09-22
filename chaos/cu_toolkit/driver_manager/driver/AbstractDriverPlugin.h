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

#ifndef CHAOSFramework_DriverWrapperPlugin_h
#define CHAOSFramework_DriverWrapperPlugin_h

#include <chaos/common/plugin/PluginDefinition.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>

namespace chaos{
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                class AbstractDriverPlugin:
                public chaos::common::plugin::AbstractPlugin {
                    AbstractDriver *abstractDriver;
                public:
                    AbstractDriverPlugin(AbstractDriver *_abstractDriver);
                    virtual ~AbstractDriverPlugin();
                    
                    DriverAccessor * const getNewAccessor();
                    
                    //! Proxy for dispose an accessor
                    void releaseAccessor(DriverAccessor *accessor);
                };
 
#define ADD_CU_DRIVER_PLUGIN_SUPERCLASS public chaos::cu::driver_manager::driver::AbstractDriver, public chaos::cu::driver_manager::driver::AbstractDriverPlugin
            
#define DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(t) t::t():chaos::cu::driver_manager::driver::AbstractDriverPlugin(this)
 
#define DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(ns, t) ns::t::t():chaos::cu::driver_manager::driver::AbstractDriverPlugin(this)
				
#define DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(a)  DEFINE_PLUGIN_DEFINITION_PROTOTYPE(a, chaos::cu::driver_manager::driver::AbstractDriverPlugin)
				
                //! Define a plugin for a driver
                /*!
                 Define a class that implement the AbstractDriver as plugin
                 \param a alias used to instantiate the driver
                 \param v the version of the driver
                 \param n the class that implement the AbstractDriver
                 */
#define OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(a, v, n)\
OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a, v, CUDRV, n, chaos::cu::driver_manager::driver::AbstractDriverPlugin)
  

#define REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE REGISTER_PLUGIN_CLASS_INIT_ATTRIBUTE

                
#define CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION CLOSE_PLUGIN_CLASS_DEFINITION
            }
        }
    }
}

#endif
