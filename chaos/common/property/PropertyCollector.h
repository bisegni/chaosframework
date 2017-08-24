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
