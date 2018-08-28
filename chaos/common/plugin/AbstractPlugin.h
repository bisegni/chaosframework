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


#ifndef CHAOSFramework_AbstractPlugin_h
#define CHAOSFramework_AbstractPlugin_h

#include <string>
#include <stdint.h>

namespace chaos {
    namespace common {
        //! Name space for grupping class for the plugin managment system of !CHAOS framework
        namespace plugin {
            
            // forward declaration
            template <typename T>
            class PluginInstancer;
            
            //! Basic information of the plugin
            typedef struct PluginInfo {
                //! the alias of the plugin used for create the instance
                std::string name;
                //! the type of the plugin
                std::string type;
                //! The version of the plugin
                std::string version;
            } PluginInfo, *PluginInfoPtr;
            
                //! Plugin abstract class
            /*!
              The base class for all other plugin. His scope is to standardize
             the sharing of the basic information of a plugin.
             */
            class AbstractPlugin {
                template <typename T>
                friend class PluginInstancer;
                
                //! Information for the current plugin
                PluginInfo info;

            public:
                AbstractPlugin(){}
                
                virtual ~AbstractPlugin(){}
                
                virtual int init(const char * init_data = NULL) {return 0;}
                
                virtual void deinit() {}
                
                const char * const getName() {return info.name.c_str();}
                
                const char * const getVersion() {return info.type.c_str();}
                
                const char * const getType() {return info.version.c_str();}
            };
            
            
            //! Permit the instantiation process of the plugin
            /*!
                This factory is used in the method created ad hoc for every plugin
                within the exported (by dll) c function named "Plugin_Name"_allocator
             */
            template <typename T>
            class PluginInstancer {
            public:
                //! Return the instance of the plugin
                /*!
                    This method return an instance of the plugin with his information filled.
                    \param name is the name of the plugin
                    \param version is the version of the plugin accordi to the type "XX.XX.XX"
                    \param type is the type of the plugin expressed by a string
                    \return the isntance of the plugin
                 */
                static T* getInstance(const char *name, const char *version, const char *type) {
                    T* instance = new T();
                    if(instance) {
                        instance->info.name = name;
                        instance->info.type = type;
                        instance->info.version = version;
                    }
                    return instance;
                }
            };
        }
    }
}
#endif
