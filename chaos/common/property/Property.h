/*
 *	Property.h
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
#ifndef CHAOSFramework_Property_h
#define CHAOSFramework_Property_h

#include <chaos/common/exception/CException.h>

#include <boost/lexical_cast.hpp>

#include <string>

#define CHAOS_PROPERTY_READ_ONLY    1
#define CHAOS_PROPERTY_WRITE_ONLY   2
#define CHAOS_PROPERTY_READ_WRITE   3

namespace chaos{
    namespace common {
        namespace property {

                //! define a property abstraction
            /*!
             this abstration this at a property like only
             string value and name and description. Subclass need to
             implement the conversion between real value and string
             */
            class AbstractProperty {
            protected:

                std::string name;

                std::string description;

            public:
                virtual void setStrValue(std::string str_value) = 0;

                virtual std::string getStrValue() = 0;

                const std::string& getName() {
                    return name;
                }

                const std::string& getDescritption() {
                    return description;
                }
            };

                //this class has been inspiered by http://www.codeproject.com/Articles/3900/Implementing-Properties-In-C
                //property entity implementation
            template <
            typename ClassOwner,
            typename ValueType,
            int AccessType >
            class Property :
            public AbstractProperty {

                    //-- Pointer to the module that contains the property --
                ClassOwner* class_owner_instance;
                    //-- Pointer to set member function --
                void (ClassOwner::*set)(ValueType value);
                    //-- Pointer to get member function --
                ValueType (ClassOwner::*get)();

            public:
                Property(){}
                ~Property(){}


                Property *defineProperty(ClassOwner* _class_owner_instance,
                                         void (ClassOwner::*_set)(ValueType value),
                                         ValueType (ClassOwner::*_get)(),
                                         const std::string& _name,
                                         const std::string& _description) {
                    class_owner_instance = _class_owner_instance;
                    name = _name;
                    description = _description;
                    set = _set;
                    get = _get;
                    return this;
                }


                    //-- Overload the = operator to set the value using the set member --
                ValueType operator =(const ValueType& value) throw (chaos::CException)  {
                    if(class_owner_instance == NULL) throw chaos::CException(-1, "No class sinstance defined", __PRETTY_FUNCTION__);
                    if((AccessType == CHAOS_PROPERTY_WRITE_ONLY) ||
                       (AccessType == CHAOS_PROPERTY_READ_WRITE)) {
                        (class_owner_instance->*set)(value);
                    }else{
                        throw chaos::CException(-1, "Variable can't be set", __PRETTY_FUNCTION__);
                    }
                    return value;
                }

                    //-- Cast the property class to the internal type --
                operator ValueType() throw (chaos::CException) {
                    assert(class_owner_instance != NULL);
                    if((AccessType == CHAOS_PROPERTY_READ_ONLY) ||
                       (AccessType == CHAOS_PROPERTY_READ_WRITE))
                        return (class_owner_instance->*get)();
                    else
                        throw chaos::CException(-1, "Variable can't be read", __PRETTY_FUNCTION__);
                }

                void setStrValue(std::string str_value) {
                    set(boost::lexical_cast<ValueType>(str_value));
                }
                
                std::string getStrValue() {
                    return boost::lexical_cast<std::string>(get());
                }
            };
        }
    }
}
#endif
