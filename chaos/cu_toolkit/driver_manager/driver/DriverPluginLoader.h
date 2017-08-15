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

#ifndef CHAOSFramework_DriverPluginLoader_h
#define CHAOSFramework_DriverPluginLoader_h
#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
namespace chaos {
    namespace cu{
        namespace driver_manager {
            namespace driver {
                
                
                //! Control Unit plugin loader
                /*!
                    PluginLoader subclass for the loading of the control unit driver plugin
                 */
                class DriverPluginLoader : private chaos::common::plugin::PluginLoader {
		  std::map<std::string, boost::function<AbstractDriverPlugin*()> > driverAllocatorFunctions;
                public:
                    //! default constructor
                    DriverPluginLoader(const char *pluginPath);
                    
                    //! default desctructor
                    ~DriverPluginLoader();
                    
                    //! give infromation about the load operation of the dll
                    bool loaded();
                    
                    //! Get the inspector for the name
		    chaos::common::plugin::PluginInspector* getInspectorForName(const char *pluginName);
                    
                    //! Return the instance of the driver
                    AbstractDriverPlugin* newDriverInstance(std::string pluginName);
                    
                    //! Return the instance of the driver
                    AbstractDriverPlugin* newDriverInstance(const char *pluginName);
                    
                };
                
            }
        }
    }
}
#endif
