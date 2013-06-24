/*
 *	AbstractPlugin.h
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


#ifndef CHAOSFramework_AbstractPlugin_h
#define CHAOSFramework_AbstractPlugin_h

#include <string>
#include <stdint.h>

namespace chaos {
    namespace common {
        namespace plugin {
            
#define SYM_ALLOC_POSTFIX   "_allocator"
#define SYM_DEALLOC_POSTFIX "_deallocator"
            
            
            using namespace std;
            
            // forward declaration
            template <typename T>
            class PluginInstancer;
            
            /*!
              Plugin abstract class
             */
            class AbstractPlugin {
                template <typename T>
                friend class PluginInstancer;
                string name;
                string version;
                string type;
            public:
                
                AbstractPlugin() {}
                
                virtual ~AbstractPlugin() {}
                
                string& getName(){return name;}
                string& getVersion(){return version;}
                string& getType(){return type;}
            };
            
            template <typename T>
            class PluginInstancer {
                string name;
                string version;
                string type;
            public:
                PluginInstancer(const char *_name, const char *_version, const char *_type):name(_name), version(_version), type(_type){}
                
                void* getInstance() {
                    AbstractPlugin* instance = static_cast<AbstractPlugin*>(new T());
                    if(instance) {
                        instance->name = name;
                        instance->version = version;
                        instance->type = type;
                    }
                    return static_cast<void*>(instance);
                }
            };
            
        }
    }
}
#endif
