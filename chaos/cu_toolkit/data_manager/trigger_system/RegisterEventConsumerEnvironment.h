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

#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEvent.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractConsumer.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractSubjectReceiver.h>

#include <boost/thread.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                namespace chaos_util = chaos::common::utility;
                namespace cu_data_trigger = chaos::cu::data_manager::trigger_system;
                
                //!define th emap taht correlata
                template<typename EventBaseClass,
                typename ConsumerBaseClass,
                typename SubjectBaseClass,
                typename EventType >
                class RegisterEventConsumerEnvironment {
                    
                    //!comodity typedef
                    typedef boost::shared_ptr< EventBaseClass >                                 EventInstanceShrdPtr;
                    typedef boost::shared_ptr< SubjectBaseClass >                               SubjectInstanceShrdPtr;
                    typedef boost::shared_ptr< ConsumerBaseClass >                              ConsumerShrdPtr;
                    typedef boost::shared_ptr< chaos_util::ObjectInstancer<ConsumerBaseClass> > ConsumerInstancerShrdPtr;

                    //!map that correlate the consumer name to the instancer
                    CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, SubjectInstanceShrdPtr, MapSubjectNameInstance);
                    CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, ConsumerInstancerShrdPtr, MapConsumerNameInstancer);
                    CHAOS_DEFINE_VECTOR_FOR_TYPE_IN_TEMPLATE(ConsumerShrdPtr, VectorConsumerInstance);
                    
                    boost::shared_mutex mutex;
                    
                    //! hav all instance of subject mapped with the UUID
                    MapSubjectNameInstance map_subject_instance;
                    
                    //!map for consumer name vs instancer
                    MapConsumerNameInstancer map_consumer_name_instancer;
                    
                    
                    //!define multiindex to permit the find operation of the aggregate event type/ subject
                    //!to forward the event to the attacched consumer
                    struct MappingEventConsumerOnSubject {
                        EventType                event_type;
                        SubjectInstanceShrdPtr   subject_instance;
                        VectorConsumerInstance   consumer_instances;
                        
                        MappingEventConsumerOnSubject(const EventType event_type,
                                                      const SubjectInstanceShrdPtr&   subject_instance){
                            
                        }
                        
                        void fireEvent(EventInstanceShrdPtr& event_to_fire) {
                            for(VectorConsumerInstanceIterator it = consumer_instances.begin(),
                                end = consumer_instances.end();
                                it != end;
                                it++) {
                                event_to_fire->executeConsumerOnTarget(subject_instance.get(),
                                                                       (*it).get());
                            }
                            
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
                    
                    SubjectInstanceShrdPtr& getContainerEntryFor(EventType event_code,
                                                                 const std::string& subject_uuid) {
                        MECTypeSubjectIndexIterator it = type_subject_index.find(boost::make_tuple(event_code,
                                                                                                   subject_uuid));
                        if(it != type_subject_index.end()) {
                            return it->second;
                        } else {
                            MappingEventConsumerOnSubjectShrdPtr mapping_instance(new MappingEventConsumerOnSubject(event_code,
                                                                                                                    map_subject_instance.get(subject_uuid)));
                            type_subject_container.insert(mapping_instance);
                            return mapping_instance;
                        }
                    }
                    
                public:
                    
                    RegisterEventConsumerEnvironment():
                    type_subject_index(type_subject_container.template get<MECTagTypeSubjectUUID>()){}
                    
                    //! register the consumer class that are sublcass of @ConsumerBaseClass
                    template<typename Consumer>
                    void registerConsumerClass(const std::string& consumer_name){
                        boost::unique_lock<boost::shared_mutex> wl(mutex);
                        ConsumerInstancerShrdPtr ci(new chaos_util::NestedObjectInstancer<ConsumerBaseClass,
                                                    cu_data_trigger::AbstractConsumer>(new chaos_util::TypedObjectInstancer<Consumer, ConsumerBaseClass>()));
                        //register instance into the map
                        map_consumer_name_instancer.insert(MapConsumerNameInstancerPair(consumer_name,
                                                                                        ci));
                    }
                    
                    //!register all subject that can be target from event
                    /*!
                     return the subject identifier that need to be used for submit event
                     */
                    const std::string& registerSubject(SubjectInstanceShrdPtr& subject_instance) {
                        boost::unique_lock<boost::shared_mutex> wl(mutex);
                        MapSubjectNameInstanceIterator it = map_subject_instance.insert(MapSubjectNameInstancePair(subject_instance->getSubjectUUID(), subject_instance));
                        return it->first;
                    }
                    
                    
                    //! register a consumer for a determinated event for a target instance
                    void addConsumerOnSubjectForEvent(EventType                 event_code,
                                                      SubjectInstanceShrdPtr&   event_taget,
                                                      const std::string&        consumer_name){
                        boost::unique_lock<boost::shared_mutex> wl(mutex);
                        //check
                        if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0) return;
                        
                        SubjectInstanceShrdPtr mapping = getContainerEntryFor(event_code,
                                                                              event_taget->getSubjectUUID());
                        
                        //allocate a new instance for the  consumer and attach it to the vector for mapping
                        ConsumerShrdPtr consumer_shared_ptr(map_subject_instance[event_taget->getSubjectUUID()]->getInstance());
                        mapping->consumer_instancer.push_back(consumer_shared_ptr);
                    }
                    
                    
                    void fireEventOnSubject(EventInstanceShrdPtr&       event_to_fire,
                                            SubjectInstanceShrdPtr&     event_subject) {
                        boost::unique_lock<boost::shared_mutex> rl(mutex);
                        SubjectInstanceShrdPtr mapping = getContainerEntryFor(event_to_fire->getType(),
                                                                              event_subject->getSubjectUUID());
                        mapping->fireEvent(event_to_fire);
                        
                    }
                };
                
            }
        }
    }
}

#endif /* RegisterEventCOnsumerEnvironment_hpp */
