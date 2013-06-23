//
//  PluginAllocator.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 6/22/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_PluginAllocator_h
#define CHAOSFramework_PluginAllocator_h

#include <string>
#include <iostream>

#include <boost/function.hpp>
#include <boost/extension/shared_library.hpp>

#include <chaos/common/plugin/AbstractPlugin.h>

namespace chaos {
    namespace plugin {
        
        using namespace std;
        using namespace boost::extensions;
        
        //! Plugin class loader and allocator
        /*!
            Plugins Allcoator class try to find th elibrary and permit to allocate instance
            of plugin class.
         */
        class PluginAllocator {
            
            //Allocator plugin exported function
            boost::function<AbstractPlugin*()> allocatorFunction;
            boost::function<void (AbstractPlugin*)> deallocatorFunction;
        public:
            PluginAllocator(const char *pluginName, const char *pluginPath) {
                //compose name
                string extensionName = pluginName;
                string allocatorName = extensionName + SYM_ALLOC_POSTFIX;
                string deallocatorName = extensionName + SYM_DEALLOC_POSTFIX;
                
                if(pluginPath) {
                    
                    //find library
                    shared_library lib(pluginPath);
                    
                    if (lib.open()) {
                        
                        //try to get function allocator
                        allocatorFunction = lib.get<AbstractPlugin*>(allocatorName);
                        deallocatorFunction = lib.get<void, AbstractPlugin*>(deallocatorName);
                    }
                }
            }
            
            bool loaded() {
                return allocatorFunction != NULL && allocatorFunction != NULL;
            }
            
            AbstractPlugin* newInstance() {
                return allocatorFunction();
            }
            
            void releaseInstance(AbstractPlugin *instance) {
                deallocatorFunction(instance);
            }
        };
        
    }
}
#endif
