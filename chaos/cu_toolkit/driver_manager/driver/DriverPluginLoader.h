/*
 *	DriverPluginLoader.h
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

#ifndef CHAOSFramework_DriverPluginLoader_h
#define CHAOSFramework_DriverPluginLoader_h

#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
namespace chaos {
    namespace cu{
        namespace dm {
            namespace driver {
                
                using namespace chaos::common::plugin;
                
                class DriverPluginLoader : private chaos::common::plugin::PluginLoader {
                    std::map<string, boost::function<AbstractDriverPlugin*()> > driverAllocatorFunctions;
                public:
                    DriverPluginLoader(const char *pluginPath);
                    
                    ~DriverPluginLoader();
                    
                     bool loaded();
                    
                    PluginInspector* getInspectorForName(const char *pluginName);
                    
                    AbstractDriverPlugin* newDriverInstance(std::string pluginName);
                    
                    AbstractDriverPlugin* newDriverInstance(const char *pluginName);
                    
                };
                
            }
        }
    }

}
#endif
