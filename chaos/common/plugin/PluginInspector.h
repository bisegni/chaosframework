/*
 *	PluginInspector.h
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

#ifndef CHAOSFramework_PluginInspector_h
#define CHAOSFramework_PluginInspector_h

#include <map>
#include <vector>
#include <iostream>
#include <chaos/common/plugin/AbstractPlugin.h>


namespace chaos {
    namespace common{
        namespace plugin {
            
            //! Postifix of the allocator
            /*!
             Define the postifx of the allocator "c" function, exported by the ddl.
             */
#define PLUGIN_INSPECTOR_POSTFIX   "_inspector"
            
            class PluginInspector {
                
                std::string name;
                std::string type;
                std::string version;
                std::string subclass;
                std::multimap<std::string, std::string> initAttributeForName;

            public:
                
                PluginInspector();
                ~PluginInspector();
                
                void setName(const char *_name);
                
                void setType(const char *_type);
                
                void setVersion(const char *_version);
                
                void setSubclass(const char *_subclass);
                
                const char * const getName();
                
                const char * const getType();
                
                const char * const getVersion();
                
                const char * const getSubclass();
                
                void addInitAttributeForName(const char *name, const char * initAttribute);
                
                size_t getInputAttributeByNamesSize(const char *name);
                
                const char * const getInputAttributeForNameAndIndex(const char *name, size_t idx);
            };
        }
    }
}

#endif

