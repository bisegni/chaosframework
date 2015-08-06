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
#include <string>
#include <vector>

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
                PropertySections map_property_sections;

            protected:
                    //! add a new property to a section
                void addPropertyToSection(const std::string& section_name,
                                          AbstractProperty *property_for_section) throw (chaos::CException);

            public:
                void setSectionPropertyStrValue(const std::string& section,
                                                const std::string& property_name,
                                                const std::string& property_value)  throw (chaos::CException);

                std::string getSectionPropertyStrValue(const std::string& section,
                                                       const std::string& property_name);
                
                void getAllSection(std::vector<std::string>& sections);
                
                void getAllPropertyForSection(const std::string& section,
                                              std::vector<std::string>& sections);
                
                const AbstractProperty *getAbstractProperty(const std::string& section,
                                                            const std::string& property);
            };
        }
    }
}

#endif
