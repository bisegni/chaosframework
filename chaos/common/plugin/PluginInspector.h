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

#ifndef CHAOSFramework_PluginInspector_h
#define CHAOSFramework_PluginInspector_h

#include <map>
#include <vector>
#include <iostream>
#include <chaos/common/plugin/AbstractPlugin.h>


namespace chaos {
    namespace common{
        namespace plugin {
            
            //! Permit the discovere of the plugin information
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
                
                PluginInspector(){}
                ~PluginInspector(){initAttributeForName.clear();}
                
                void setName(const char *_name) {
                    name = _name;
                }

                void setType(const char *_type) {
                    type = _type;
                }
                
                void setVersion(const char *_version) {
                    version = _version;
                }
                
                void setSubclass(const char *_subclass) {
                    subclass = _subclass;
                }
                
                const char * const getName() {
                    return name.c_str();
                }
                
                const char * const getType() {
                    return type.c_str();
                }
                
                const char * const getVersion() {
                    return version.c_str();
                }
                
                const char * const getSubclass() {
                    return subclass.c_str();
                }
                
                void addInitAttributeForName(const char *name, const char * initAttribute) {
                    initAttributeForName.insert(std::make_pair<std::string, std::string>(name, initAttribute));
                }
                
                size_t getInputAttributeByNamesSize(const char *name) {
                    return initAttributeForName.count(name);
                }
                
                const char * const getInputAttributeForNameAndIndex(const char *name, size_t idx) {
                    std::vector<std::string> initAttributes;
                    //check if name exists
                    if(!initAttributeForName.count(name)) return NULL;
                    
                    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> attributesIterator;
                    attributesIterator = initAttributeForName.equal_range(name);
                    
                    //copy all init attribute for name
                    int _idx = 0;
                    for (std::multimap<std::string, std::string>::iterator it2 = attributesIterator.first;
                         it2 != attributesIterator.second;
                         ++it2) {
                        if(idx==_idx++) return it2->second.c_str();
                    }
                    return NULL;
                }

            };
        }
    }
}

#endif

