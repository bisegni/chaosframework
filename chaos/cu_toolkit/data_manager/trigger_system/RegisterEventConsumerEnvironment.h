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
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractSubject.h>

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
                
                //Base trigger environment
                class TriggerEnvironment {
                    const std::string environment_name;
                public:
                    TriggerEnvironment(const std::string& _environment_name):
                    environment_name(_environment_name){}
                    ~TriggerEnvironment(){}
                    
                    const std::string& getEnvironmentName() {
                        return environment_name;
                    }
                    
                    virtual std::auto_ptr<chaos::common::data::CDataWrapper> serialize() = 0;
                };
                
                //!define th emap taht correlata
                template<typename EventBaseClass,
                typename ConsumerBaseClass,
                typename SubjectBaseClass,
                typename EventType >
                class RegisterEventConsumerEnvironment:
                public TriggerEnvironment{
                public:
                    //!comodity typedef
                    typedef boost::shared_ptr< EventBaseClass >     EventInstanceShrdPtr;
                    typedef boost::shared_ptr< SubjectBaseClass >   SubjectInstanceShrdPtr;
                    typedef boost::shared_ptr< ConsumerBaseClass >  ConsumerShrdPtr;
                    
                    typedef boost::shared_ptr< ConsumerInstanceDescription<ConsumerBaseClass> > ConsumerInstancerShrdPtr;
                    typedef boost::shared_ptr< ConsumerInstanceDescription<EventBaseClass> >    EventInstancerShrdPtr;
                protected:
                    //!map for data
                    CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, SubjectInstanceShrdPtr,      MapSubjectNameInstance);
                    CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, ConsumerInstancerShrdPtr,    MapConsumerNameInstancer);
                    CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(std::string, EventInstancerShrdPtr,       MapEventNameInstancer);
                    CHAOS_DEFINE_VECTOR_FOR_TYPE_IN_TEMPLATE(ConsumerShrdPtr, VectorConsumerInstance);
                    
                    boost::shared_mutex mutex;
                    
                    //! hav all instance of subject mapped with the UUID
                    MapSubjectNameInstance map_subject_instance;
                    
                    //!map for consumer name vs instancer
                    MapConsumerNameInstancer map_consumer_name_instancer;
                    
                    MapEventNameInstancer map_event_name_instancer;
                    
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
                    
                    VectorConsumerInstance& getConsumerInstanceListFor(EventType event_code,
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
                    
                    ConsumerResult fireEvent(const EventType&               event_code,
                                             const std::string&             subject_uuid,
                                             const EventInstanceShrdPtr&    event_to_fire) {
                        MECTypeSubjectIndexIterator it = type_subject_index.find(boost::make_tuple(event_code,
                                                                                                   subject_uuid));
                        if(it == type_subject_index.end()) return ConsumerResultOK;
                        return (*it)->fireEvent(event_to_fire);
                    }
                public:
                    
                    RegisterEventConsumerEnvironment(const std::string& _environment_name):
                    TriggerEnvironment(_environment_name),
                    type_subject_index(type_subject_container.template get<MECTagTypeSubjectUUID>()){}
                    
                    ~RegisterEventConsumerEnvironment(){}
                    
                    
                    
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
                        map_event_name_instancer.insert(MapEventNameInstancerPair(desc_shrd_ptr->getGroupName(),
                                                                                  desc_shrd_ptr));
                    }
                    
                    EventInstanceShrdPtr getEventInstance(const std::string& event_name) {
                        if(map_event_name_instancer[event_name] == 0) return EventInstanceShrdPtr();
                        
                        //we have the instancer so we can create the instance of event
                        return EventInstanceShrdPtr(map_event_name_instancer[event_name]->getInstance());
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
                    
                    
                    //! register a consumer for a determinated event for a target instance
                    bool addConsumerOnSubjectForEvent(EventType                 event_code,
                                                      SubjectInstanceShrdPtr&   event_taget,
                                                      const std::string&        consumer_name,
                                                      const MapKeyCDataVariant  *consumer_parameter = NULL){
                        boost::unique_lock<boost::shared_mutex> wl(mutex);
                        //check
                        if(map_subject_instance.count(event_taget->getSubjectUUID()) == 0 ||
                           map_consumer_name_instancer[consumer_name] == 0) return false;
                        
                        VectorConsumerInstance& consumer_list_ref = getConsumerInstanceListFor(event_code,
                                                                                               event_taget->getSubjectUUID());
                        
                        //allocate a new instance for the  consumer and attach it to the vector for mapping
                        consumer_list_ref.push_back(ConsumerShrdPtr(map_consumer_name_instancer[consumer_name]->getInstance()));
                        
                        return true;
                    }
                    
                    
                    ConsumerResult fireEventOnSubject(const EventInstanceShrdPtr&       event_to_fire,
                                                      const SubjectInstanceShrdPtr&     event_subject) {
                        boost::unique_lock<boost::shared_mutex> rl(mutex);
                        return fireEvent(static_cast<EventType>(event_to_fire->getEventCode()),
                                         event_subject->getSubjectUUID(),
                                         event_to_fire);
                    }
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                        std::auto_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                        //describe subject
                        //describe event
                        //describe consumer
                        return result;
                    }
                    
                };
                
            }
        }
    }
}

#endif /* RegisterEventCOnsumerEnvironment_hpp */
