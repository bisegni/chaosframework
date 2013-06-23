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
    namespace plugin {
        
#define SYM_ALLOC_POSTFIX   "_allocator"
#define SYM_DEALLOC_POSTFIX "_deallocator"
        
        
        using namespace std;
        
        template <typename T>
        class PluginInstancer {
        public:
            T* getInstance() {
                new T();
            }
        };
        
        class AbstractPlugin {
            
        public:
            
            AbstractPlugin() {}
            
            virtual ~AbstractPlugin() {}
            
        };
        
    }
}
#endif
