/*
 *	DatasetAttributeHandler.h
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

#ifndef __CHAOSFramework__DatasetAttributeHandler_h
#define __CHAOSFramework__DatasetAttributeHandler_h

#include <chaos/cu_toolkit/control_manager/handler/AbstractAttributeHandler.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace handler {
                
#define DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(n,t)\
template<typename O>\
class n:\
public chaos::cu::control_manager::handler::DatasetAttributeHandlerDescription<O, t> {\
public:\
n(const std::string& _attribute_name,\
O* _object_reference,\
typename chaos::cu::control_manager::handler::DatasetAttributeHandlerDescription<O, t>::HandlerDescriptionActionPtr _handler_pointer){}\
};
                
                
                //! base dataset attribute class
                class AbstractHandlerDescription {
                protected:
                    const std::string attribute_name;
                public:
                    AbstractHandlerDescription(const std::string& _attribute_name):
                    attribute_name(_attribute_name){};
                    virtual bool executeHandler(chaos::common::data::CDataWrapper *attribute_changes_set) = 0;
                };
                
                template<typename O, typename T>
                class DatasetAttributeHandlerDescription:
                public AbstractHandlerDescription {
                public:
                    typedef bool (O::*HandlerDescriptionActionPtr)(const std::string& attribute_name,
                                                                   const T value,
                                                                   const uint32_t value_size);
                    
                    DatasetAttributeHandlerDescription(O* _object_reference,
                                                       HandlerDescriptionActionPtr _handler_pointer,
                                                       const std::string& _attribute_name):
                    AbstractHandlerDescription(_attribute_name),
                    object_reference(_object_reference),
                    handler_pointer(_handler_pointer){}
                    
                protected:
                    
                    
                    bool executeHandler(chaos::common::data::CDataWrapper *attribute_changes_set) {
                        if(attribute_changes_set == NULL ||
                           attribute_changes_set->hasKey(attribute_name) == false) return false;
                        //broadcast the attribute value
                        return ((*object_reference).*handler_pointer)(attribute_name,
                                                                      attribute_changes_set->getValue<T>(attribute_name),
                                                                      attribute_changes_set->getValueSize(attribute_name));
                    }
                    
                private:
                    O* object_reference;
                    HandlerDescriptionActionPtr handler_pointer;
                };
                
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(BoolDAHD, bool);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(Int32DAHD, int32_t);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(Int64DAHD, int64_t);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(UInt32DAHD, uint32_t);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(UInt64DAHD, uint64_t);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(DoubleDAHD, double);
                DEFINE_HANDLER_DESCRIPTION_FOR_TYPE(CharPtrDAHD, const char *);
                
                //!
                typedef boost::shared_ptr<AbstractHandlerDescription> HandlerDescriptionPtr;
                
                //!
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                                          boost::shared_ptr<AbstractHandlerDescription>,
                                          MapAttributeHandler);
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string,
                                             HandlerAssociatedAttributeList);
                
                
                template<typename O>
                class DatasetAttributeVariantHandlerDescription:
                public AbstractHandlerDescription {
                public:
                    typedef bool (O::*HandlerDescriptionActionPtr)(const std::string& attribute_name,
                                                                   const chaos::common::data::CDataVariant& value);
                    
                    DatasetAttributeVariantHandlerDescription(O* _object_reference,
                                                              HandlerDescriptionActionPtr _handler_pointer,
                                                              const std::string& _attribute_name):
                    AbstractHandlerDescription(_attribute_name),
                    object_reference(_object_reference),
                    handler_pointer(_handler_pointer){}
                    
                protected:
                    
                    
                    bool executeHandler(chaos::common::data::CDataWrapper *attribute_changes_set) {
                        if(attribute_changes_set == NULL) return false;
                        //broadcast the attribute value
                        return ((*object_reference).*handler_pointer)(attribute_name,
                                                                      attribute_changes_set->getVariantValue(attribute_name));
                    }
                    
                private:
                    O* object_reference;
                    HandlerDescriptionActionPtr handler_pointer;
                };
                
                struct HandlerSetElement {
                    HandlerDescriptionPtr handler_ptr;
                };
                
                //!
                class DatasetAttributeHandler:
                public AbstractAttributeHandler {
                    MapAttributeHandler map_handlers_for_attribute;
                public:
                    DatasetAttributeHandler();
                    ~DatasetAttributeHandler();
                    
                    template<typename O, typename T>
                    bool addHandlerOnAttributeName(O *object_reference,
                                                   const std::string& attribute_name,
                                                   typename DatasetAttributeHandlerDescription<O,T>::HandlerDescriptionActionPtr handler_ptr) {
                        
                        
                        if(map_handlers_for_attribute.count(attribute_name) != 0 ||
                           handler_ptr == NULL) return false;
                        
                        //register the handler
                        map_handlers_for_attribute.insert(make_pair(attribute_name, HandlerDescriptionPtr(new DatasetAttributeHandlerDescription<O,T>(object_reference,
                                                                                                                                                      handler_ptr,
                                                                                                                                                      attribute_name))));
                        return true;
                    }
                    
                    template<typename O>
                    bool addVariantHandlerOnAttributeName(O *object_reference,
                                                          const std::string& attribute_name,
                                                          typename DatasetAttributeVariantHandlerDescription<O>::HandlerDescriptionActionPtr handler_ptr) {
                        
                        
                        if(map_handlers_for_attribute.count(attribute_name) != 0 ||
                           handler_ptr == NULL) return false;
                        
                        //register the handler
                        map_handlers_for_attribute.insert(make_pair(attribute_name, HandlerDescriptionPtr(new DatasetAttributeVariantHandlerDescription<O>(object_reference,
                                                                                                                                                           handler_ptr,
                                                                                                                                                           attribute_name))));
                        return true;
                    }
                    
                    bool removeHandlerOnAttributeName(const std::string& attribute_name);
                    
                    void clearAllAttributeHandler();
                    
                    void executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__DatasetAttributeHandler_h */
