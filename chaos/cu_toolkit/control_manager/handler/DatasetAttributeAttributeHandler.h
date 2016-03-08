/*
 *	DatasetAttributeAttributeHandler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 08/03/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DatasetAttributeAttributeHandler_h
#define __CHAOSFramework__DatasetAttributeAttributeHandler_h

#include <chaos/cu_toolkit/control_manager/handler/AbstractAttributeHandler.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace handler {
                
                class AbstractHandlerDescription {
                    const std::string& attribute_name;
                public:
                    AbstractHandlerDescription(const std::string& _attribute_name):
                    attribute_name(_attribute_name){};
                    virtual bool executeHandler(chaos::common::data::CDataWrapper *attribute_changes_set) = 0;
                };
                
                template<typename O>
                class DatasetInt32AttributeHandlerDescription:
                public AbstractHandlerDescription {
                public:
                    typedef bool (O::*HandlerDescriptionActionPtr)(const std::string& attribute_name,
                                                                   const int32_t attribute_value);
                protected:
                    DatasetInt32AttributeHandlerDescription(O* _object_reference,
                                                            HandlerDescriptionActionPtr _handler_pointer,
                                                            const std::string& _attribute_name):
                    AbstractHandlerDescription(_attribute_name),
                    object_reference(_object_reference),
                    handler_pointer(_handler_pointer){}
                    
                    bool executeHandler(chaos::common::data::CDataWrapper *attribute_changes_set) {
                        if(attribute_changes_set == NULL ||
                           attribute_changes_set->hasKey(attribute_name) == false) return false;
                        
                        //broadcast the attribute value

                        if(attribute_changes_set->isBoolValue(attribute_name) == false) return false;
                        return ((*object_reference).*handler_pointer)(attribute_name, attribute_changes_set->getInt32Value(attribute_name));
                    }
                    
                private:
                    O* object_reference;
                    HandlerDescriptionActionPtr handler_pointer;
                    const std::string attribute_name;
                };
                
                typedef boost::shared_ptr<AbstractHandlerDescription> HandlerPointer;
                
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, HandlerPointer, MapAttributeHandler);
                
                class DatasetAttributeAttributeHandler:
                public AbstractAttributeHandler {
                    MapAttributeHandler map_handlers_for_attribute;
                protected:
                    void executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set);
                public:
                    DatasetAttributeAttributeHandler();
                    ~DatasetAttributeAttributeHandler();
                    
                    template<typename O, typename T>
                    bool addHandlerOnAttributeName(const std::string& attribute_name,
                                                   AbstractHandlerDescription* handler_pointer) {
                        if(map_handlers_for_attribute.count(attribute_name) == 0 ||
                           handler_pointer == NULL) return false;
                        map_handlers_for_attribute.insert(make_pair(attribute_name, HandlerPointer(handler_pointer)));
                    }
                    bool removeHandlerOnAttributeName(const std::string& attribute_name);
                    
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__DatasetAttributeAttributeHandler_h */
