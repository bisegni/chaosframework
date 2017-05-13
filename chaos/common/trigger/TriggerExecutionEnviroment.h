/*
 *	EventConsumerDatasetChange.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A5E9B423_6474_43C8_92B3_A4FCA8AC8C77_RegisterEventConsumerEnvironment_h
#define __CHAOSFramework__A5E9B423_6474_43C8_92B3_A4FCA8AC8C77_RegisterEventConsumerEnvironment_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/ObjectInstancer.h>

#include <chaos/common/trigger/AbstractEvent.h>
#include <chaos/common/trigger/AbstractConsumer.h>
#include <chaos/common/trigger/AbstractSubject.h>

#include <boost/thread.hpp>
#include <boost/make_shared.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace chaos {
    namespace common {
        namespace trigger {
            
            namespace chaos_util = chaos::common::utility;
            namespace common_trigger = chaos::common::trigger;
            
            //Base trigger environment
            class AbstractTriggerEnvironment {
                const std::string environment_name;
            public:
                AbstractTriggerEnvironment(const std::string& _environment_name):
                environment_name(_environment_name){}
                ~AbstractTriggerEnvironment(){}
                
                const std::string& getEnvironmentName() {
                    return environment_name;
                }
                
                virtual std::unique_ptr<chaos::common::data::CDataWrapper> serialize() = 0;
            };
            
            //!define th emap taht correlata
            template<typename SubjectBaseClass,
            typename EventType >
            class TriggerExecutionEnviroment:
            public AbstractTriggerEnvironment {
            public:
                //!comodity typedef
                typedef boost::shared_ptr< SubjectBaseClass >                 SubjectInstanceShrdPtr;
                typedef typename boost::shared_ptr< typename EventInstancerDescription<EventType, SubjectBaseClass>::ConcreteEvent >        EventInstanceShrdPtr;
                typedef typename boost::shared_ptr< typename ConsumerInstancerDescription<EventType, SubjectBaseClass>::ConcreteConsumer >  ConsumerShrdPtr;
                
                typedef typename ConsumerInstancerDescription<EventType, SubjectBaseClass>::ConsumerInstancerShrdPtr                ConsumerInstancerShrdPtr;
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::EventInstancerShrdPtr                      EventInstancerShrdPtr;
                
                //!map for data
                CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, SubjectInstanceShrdPtr,      MapSubjectNameInstance);
                CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, ConsumerInstancerShrdPtr,    MapConsumerNameInstancer);
                CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, EventInstancerShrdPtr,       MapEventNameInstancer);
                CHAOS_DEFINE_VECTOR_FOR_TYPE_IN_TEMPLATE(ConsumerShrdPtr, VectorConsumerInstance);
            protected:
                boost::shared_mutex mutex;
                
                //! hav all instance of subject mapped with the UUID
                MapSubjectNameInstance map_subject_instance;
                
                //!map for consumer name vs instancer
                MapConsumerNameInstancer map_consumer_name_instancer;
                
                //! container for event descriptor
                typename EventInstancerDescription<EventType, SubjectBaseClass>::EventDescriptionContainer      event_descriptor_container;
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TriggerEventTagCode    TriggerEventTagCode;
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TriggerEventTagName    TriggerEventTagName;
                
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TENameIndex            EventInstanceDescriptorNameIndex;
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TENameIndexIterator    EventInstanceDescriptorNameIndexIterator;
                
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TECodeIndex            EventInstanceDescriptorCodeIndex;
                typedef typename EventInstancerDescription<EventType, SubjectBaseClass>::TECodeIndexIterator    EventInstanceDescriptorCodeIndexIterator;
                //!define multiindex to permit the find operation of the aggregate event type/ subject
                //!to forward the event to the attacched consumer
                struct MappingEventConsumerOnSubject {
                    EventType                event_type;
                    SubjectInstanceShrdPtr   subject_instance;
                    VectorConsumerInstance   consumer_instances;
                    
                    MappingEventConsumerOnSubject(const EventType _event_type,
                                                  const SubjectInstanceShrdPtr&   subject_instance):
                    event_type(_event_type),
                    subject_instance(subject_instance){}
                    
                    ConsumerResult fireEvent(const EventInstanceShrdPtr& event_to_fire) {
                        ConsumerResult result = ConsumerResultOK;
                        VectorConsumerInstanceIterator it = consumer_instances.begin();
                        VectorConsumerInstanceIterator end = consumer_instances.end();
                        while(it != end &&
                              result != ConsumerResultCritical){
                            //update consuer properties
                            (*it)->updateProperty(*event_to_fire);
                            
                            //execute consumer phase
                            result = event_to_fire->executeConsumerOnTarget(subject_instance.get(),
                                                                            (*it++).get());
                        }
                        return result;
                    }
                };
                
                typedef boost::shared_ptr< MappingEventConsumerOnSubject > MappingEventConsumerOnSubjectShrdPtr;
                
                //!multi index key extractor
                struct AbstractEventMIExstractEventType {
                    typedef EventType result_type;
                    const result_type &operator()(const MappingEventConsumerOnSubjectShrdPtr &p) const {
                        return p->event_type;
                    }
                };
                
                struct AbstractEventMIExstractSubjectUUID {
                    typedef std::string result_type;
                    const result_type &operator()(const MappingEventConsumerOnSubjectShrdPtr &p) const {
                        return p->subject_instance->getSubjectUUID();
                    }
                };
                
                struct MECTagTypeSubjectUUID{};
                
                typedef boost::multi_index_container<
                MappingEventConsumerOnSubjectShrdPtr,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique< boost::multi_index::tag<MECTagTypeSubjectUUID>, boost::multi_index::composite_key<
                MappingEventConsumerOnSubjectShrdPtr,
                AbstractEventMIExstractEventType,
                AbstractEventMIExstractSubjectUUID
                >
                >
                >
                > MappingEventConsumerContainer;
                
                
                typedef typename boost::multi_index::index<MappingEventConsumerContainer, MECTagTypeSubjectUUID>::type              MECTypeSubjectIndex;
                typedef typename boost::multi_index::index<MappingEventConsumerContainer, MECTagTypeSubjectUUID>::type::iterator    MECTypeSubjectIndexIterator;
                
                MappingEventConsumerContainer   type_subject_container;
                MECTypeSubjectIndex&            type_subject_index;
                
                inline VectorConsumerInstance& getConsumerInstanceListFor(EventType event_code,
                                                                          const std::string& subject_uuid) {
                    MECTypeSubjectIndexIterator it = type_subject_index.find(boost::make_tuple(event_code,
                                                                                               subject_uuid));
                    if(it != type_subject_index.end()) {
                        return (*it)->consumer_instances;
                    } else {
                        MappingEventConsumerOnSubjectShrdPtr mapping_instance(boost::make_shared<MappingEventConsumerOnSubject>(event_code,
                                                                                                                                map_subject_instance[subject_uuid]));
                        type_subject_container.insert(mapping_instance);
                        return mapping_instance->consumer_instances;
                    }
                }
                
                inline ConsumerShrdPtr getConsumerFromList(EventType                                       event_code,
                                                           SubjectInstanceShrdPtr&                         event_taget,
                                                           const std::string&                              consumer_uuid){
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //check
                    if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0) return ConsumerShrdPtr();
                    
                    //get all consumer
                    VectorConsumerInstance& consumer_list_ref = getConsumerInstanceListFor(event_code,
                                                                                           event_taget->getSubjectUUID());
                    
                }
                
                ConsumerResult fireEvent(const EventType&               event_code,
                                         const std::string&             subject_uuid,
                                         const EventInstanceShrdPtr&    event_to_fire) {
                    MECTypeSubjectIndexIterator it = type_subject_index.find(boost::make_tuple(event_code,
                                                                                               subject_uuid));
                    if(it == type_subject_index.end()) return ConsumerResultOK;
                    return (*it)->fireEvent(event_to_fire);
                }
            public:
                
                TriggerExecutionEnviroment(const std::string& _environment_name):
                AbstractTriggerEnvironment(_environment_name),
                type_subject_index(type_subject_container.template get<MECTagTypeSubjectUUID>()){}
                
                ~TriggerExecutionEnviroment(){}
                
                
                
                //! register the consumer class that are sublcass of @ConsumerBaseClass
                template<typename ConsumerDescriptor>
                void registerConsumerClass(){
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //register instance into the map
                    ConsumerInstancerShrdPtr desc_shrd_ptr(new ConsumerDescriptor());
                    map_consumer_name_instancer.insert(MapConsumerNameInstancerPair(desc_shrd_ptr->getGroupName(),
                                                                                    desc_shrd_ptr));
                }
                
                template<typename EventDescriptor>
                void registerEventClass(){
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //register instance into the map
                    EventInstancerShrdPtr desc_shrd_ptr(new EventDescriptor());
                    event_descriptor_container.insert(desc_shrd_ptr);
                }
                
                EventInstanceShrdPtr getEventInstance(const std::string& event_name) {
                    EventInstanceDescriptorNameIndex& index = event_descriptor_container.template get<TriggerEventTagName>();
                    EventInstanceDescriptorNameIndexIterator index_it = index.find(event_name);
                    if(index_it == index.end()) return EventInstanceShrdPtr();
                    
                    //we have the instancer so we can create the instance of event
                    return EventInstanceShrdPtr((*index_it)->getInstance());
                }
                
                EventInstanceShrdPtr getEventInstance(const EventType& event_type) {
                    EventInstanceDescriptorCodeIndex& index = event_descriptor_container.template get<TriggerEventTagCode>();
                    EventInstanceDescriptorCodeIndexIterator index_it = index.find(event_type);
                    if(index_it == index.end()) return EventInstanceShrdPtr();
                    
                    //we have the instancer so we can create the instance of event
                    return EventInstanceShrdPtr((*index_it)->getInstance());
                }
                
                //!register all subject that can be target from event
                /*!
                 return the subject identifier that need to be used for submit event
                 */
                const std::string& registerSubject(SubjectInstanceShrdPtr& subject_instance) {
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    map_subject_instance.insert(MapSubjectNameInstancePair(subject_instance->getSubjectUUID(), subject_instance));
                    return subject_instance->getSubjectUUID();
                }
                
                
                bool addConsumerOnSubjectForEvent(EventType                 event_code,
                                                  SubjectInstanceShrdPtr&   event_taget,
                                                  const std::string&        consumer_name){
                    std::string new_consumer_uuid;
                    return addConsumerOnSubjectForEvent(event_code,
                                                        event_taget,
                                                        consumer_name,
                                                        new_consumer_uuid);
                }
                
                //! register a consumer for a determinated event for a target instance
                bool addConsumerOnSubjectForEvent(EventType                                     event_code,
                                                  SubjectInstanceShrdPtr&                       event_taget,
                                                  const std::string&                            consumer_name,
                                                  std::string&                                  new_consumer_uuid){
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //check
                    if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0 ||
                       map_consumer_name_instancer[consumer_name] == 0) return false;
                    //reset the string
                    new_consumer_uuid.clear();
                    VectorConsumerInstance& consumer_list_ref = getConsumerInstanceListFor(event_code,
                                                                                           event_taget->getSubjectUUID());
                    
                    //allocate a new instance for the  consumer and attach it to the vector for mapping
                    ConsumerShrdPtr new_consumer_instance(map_consumer_name_instancer[consumer_name]->getInstance());
                    consumer_list_ref.push_back(new_consumer_instance);
                    //get new consumer uuid
                    new_consumer_uuid = new_consumer_instance->getConsumerUUID();
                    return true;
                }
                
                bool updateConsumerPropertiesBy(EventType                                       event_code,
                                                SubjectInstanceShrdPtr&                         event_taget,
                                                const std::string&                              consumer_uuid,
                                                chaos::common::property::PropertyGroup&         consumer_properties){
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //check
                    if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0) return false;
                    
                    //get all consumer
                    ConsumerShrdPtr consumer_shrd_ptr = getConsumerFromList(event_code,
                                                                            event_taget->getSubjectUUID(),
                                                                            consumer_uuid);
                    if(consumer_shrd_ptr.get()) {
                        consumer_shrd_ptr->updateProperty(consumer_properties);
                    }
                    
                }
                
                bool getConsumerListBy(EventType                   event_code,
                                       SubjectInstanceShrdPtr&     event_taget,
                                       VectorConsumerInstance&     attached_consumer) {
                    boost::unique_lock<boost::shared_mutex> wl(mutex);
                    //check
                    if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0) return false;
                    
                    //get all consumer
                    VectorConsumerInstance& consumer_list_ref = getConsumerInstanceListFor(event_code,
                                                                                           event_taget->getSubjectUUID());
                    
                    //fill vector with all consumer instance
                    attached_consumer.insert(attached_consumer.end(),
                                             consumer_list_ref.begin(),
                                             consumer_list_ref.end());
                    return true;
                }
                
                ConsumerResult fireEventOnSubject(const EventInstanceShrdPtr&       event_to_fire,
                                                  const SubjectInstanceShrdPtr&     event_subject) {
                    boost::unique_lock<boost::shared_mutex> rl(mutex);
                    return fireEvent(static_cast<EventType>(event_to_fire->getEventCode()),
                                     event_subject->getSubjectUUID(),
                                     event_to_fire);
                }
                
                std::unique_ptr<chaos::common::data::CDataWrapper> serialize() {
                    std::unique_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                    
                    common::property::PropertyGroup tmp_property_group;
                    common::property::PropertyGroupSDWrapper group_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(common::property::PropertyGroup, tmp_property_group));
                    
                    //add enviroment_name
                    result->addStringValue("trg_env_name", getEnvironmentName());
                    
                    //describe subjects
                    for(MapSubjectNameInstanceIterator it = map_subject_instance.begin(),
                        end = map_subject_instance.end();
                        it != end;
                        it++) {
                        result->appendStringToArray(it->second->getSubjectName());
                    }
                    result->finalizeArrayForKey("trg_env_subjects");
                    
                    //describe event
                    EventInstanceDescriptorCodeIndex& event_code_index = event_descriptor_container.template get<TriggerEventTagCode>();
                    for(EventInstanceDescriptorCodeIndexIterator it = event_code_index.begin(),
                        end = event_code_index.end();
                        it != end;
                        it++) {
                        group_ref_wrapper() = *(*it);
                        result->appendCDataWrapperToArray(*group_ref_wrapper.serialize());
                        
                    }
                    result->finalizeArrayForKey("trg_env_events");
                    
                    //describe consumer
                    for(MapConsumerNameInstancerIterator it = map_consumer_name_instancer.begin(),
                        end = map_consumer_name_instancer.end();
                        it != end;
                        it++) {
                        group_ref_wrapper() = *it->second;
                        result->appendCDataWrapperToArray(*group_ref_wrapper.serialize());
                        
                    }
                    result->finalizeArrayForKey("trg_env_consumers");
                    return result;
                }
                
            };
        }
    }
}

#endif /* RegisterEventCOnsumerEnvironment_hpp */
