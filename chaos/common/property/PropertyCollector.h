/*
 *	PropertyCollector.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_PropertyCollector_h
#define CHAOSFramework_PropertyCollector_h


#include <chaos/common/property/Property.h>

#include <map>

#define DECLARE_CHAOS_PROPERTY(own, type, access_type, name)\
chaos::common::property::Property<own, type, access_type> name;

#define REGISTER_CHAOS_PROPERTY(section, name, set, get, description)\
addPropertyToSection(section, name.defineProperty(this, set, get, #name, description));


namespace chaos{
    namespace common {
            //! nameapce for the managment of the !CHAOS publishable properties.

        namespace property {

                //! define the list of the properties
            typedef std::map<std::string, AbstractProperty* >   PropertyMap;
                //! define the property sections
            typedef std::map<std::string, PropertyMap >   PropertySections;

                //! a collector for all calss or subclass property
            /*!
             Every class that extends the collector can register property(with rule, get and setter) that
             will be published on metadata server and can be directly updated remotely.
             */
            class PropertyCollector {
                    //!associate a section name to a property list
                PropertySections property_sections;

            protected:
                    //! add a new property to a section
                void addPropertyToSection(const std::string& section_name,
                                          AbstractProperty *property_for_section) throw (chaos::CException) {
                    if(property_sections.count(section_name) == 0) {
                        property_sections.insert(make_pair(section_name, PropertyMap()));
                    }
                    if(property_sections[section_name].count(property_for_section->getName()) > 0) {
                        throw chaos::CException(-1, "Property already registerd", __PRETTY_FUNCTION__);
                    }
                    property_sections[section_name].insert(make_pair(property_for_section->getName(), property_for_section));
                }

            public:
                void setSectionProperty(const std::string& section,
                                        const std::string& property_name,
                                        const std::string& property_value)  throw (chaos::CException) {
                    if((property_sections.count(section) > 0) &&
                       (property_sections[section].count(property_name) > 0)){
                        property_sections[section][property_name]->setStrValue(property_value);
                    }
                }

                std::string getSectionProperty(const std::string& section,
                                               const std::string& property_name) {
                    if((property_sections.count(section) > 0) &&
                       (property_sections[section].count(property_name) > 0)){
                        return property_sections[section][property_name]->getStrValue();
                    } else {
                        return std::string();
                    }
                }
                
            };
        }
    }
}

#endif
