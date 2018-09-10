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
#ifndef __CHAOSFramework__WorkUnitManagement__
#define __CHAOSFramework__WorkUnitManagement__

#include <chaos/common/data/CDataWrapper.h>

#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <boost/thread.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>
#include <chaos/common/message/MDSMessageChannel.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace work_unit_state_machine {
                    //Work unit event type
                namespace UnitEventType {
                        // wotkunit is instatiated e all information are got
                    struct UnitEventTypeUnpublished {};
                        // start the publishing porcess for the workunit
                    struct UnitEventTypePublish {};
                        // work unit is retrying to register hisself to the mds
                    struct UnitEventTypePublishing {};
                        // unit server is sucessfully registered
                    struct UnitEventTypePublished {};
                        // unit server is starting the unpublishjing process
                    struct UnitEventTypeUnpublish {};
                        // unit server is waiting the unpublishing completion process
                    struct UnitEventTypeUnpublishing {};
                        // unit server is failed to register to the mds
                    struct UnitEventTypeFailure {};
                        //reset state machine
                    struct UnitEventTypeReset {};
                }

                    //! work unit is not published
                struct Unpublished : public boost::msm::front::state<>{};
                    //! work unit is publishing to the mds service
                struct StartPublishing : public boost::msm::front::state<> {};
                    //! work unit is publishing to the mds service
                struct Publishing : public boost::msm::front::state<> {};
                    //! work unit has been published, and his heartbeat start to pulse
                struct Published : public boost::msm::front::state<> {};
                    //! workunit is beginning to unpublish to the mds
                struct StartUnpublishing : public boost::msm::front::state<> {};
                    //! unpublishing event is given and now work unit start te unpublishing process
                struct Unpublishing : public boost::msm::front::state<> {};
                    //! there was been some failure during publishing operation
                struct PublishingFailure  : public boost::msm::front::state<> {};

                    // front-end: define the FSM structure
                struct wu_state_machine : public boost::msm::front::state_machine_def<wu_state_machine> {

                        // the initial state of the player SM. Must be defined
                    typedef Unpublished initial_state;
                    typedef boost::msm::front::Row <  Unpublished		,  UnitEventType::UnitEventTypePublish		, StartPublishing	, boost::msm::front::none , boost::msm::front::none > unpub_spubing_row;
                    typedef boost::msm::front::Row <  StartPublishing   ,  UnitEventType::UnitEventTypePublishing	, Publishing		, boost::msm::front::none , boost::msm::front::none > spubing_pubing_row;
                    typedef boost::msm::front::Row <  Publishing		,  UnitEventType::UnitEventTypePublished	, Published			, boost::msm::front::none , boost::msm::front::none > pubing_pub_row;
                    typedef boost::msm::front::Row <  Published			,  UnitEventType::UnitEventTypeUnpublish	, StartUnpublishing	, boost::msm::front::none , boost::msm::front::none > pub_sunpubing_row;
                    typedef boost::msm::front::Row <  StartUnpublishing ,  UnitEventType::UnitEventTypeUnpublishing , Unpublishing		, boost::msm::front::none , boost::msm::front::none > sunpubing_unpubing_row;
                    typedef boost::msm::front::Row <  Unpublishing		,  UnitEventType::UnitEventTypeUnpublished  , Unpublished		, boost::msm::front::none , boost::msm::front::none > unpubing_unpub_row;
                    typedef boost::msm::front::Row <  Publishing		,  UnitEventType::UnitEventTypeFailure		, PublishingFailure	, boost::msm::front::none , boost::msm::front::none > pubbing_pubfail_row;
                    typedef boost::msm::front::Row <  StartPublishing	,  UnitEventType::UnitEventTypeFailure		, PublishingFailure	, boost::msm::front::none , boost::msm::front::none > spubing_pubfail_row;
                    typedef boost::msm::front::Row <  PublishingFailure	,  UnitEventType::UnitEventTypeReset		, Unpublished       , boost::msm::front::none , boost::msm::front::none > pubfail_unpub_row;
                        // Transition table for initialization services
                    struct transition_table : boost::mpl::vector<
                    unpub_spubing_row,
                    spubing_pubing_row,
                    pubing_pub_row,
                    pub_sunpubing_row,
                    sunpubing_unpubing_row,
                    unpubing_unpub_row,
                    pubbing_pubfail_row,
                    spubing_pubfail_row,
                    pubfail_unpub_row> {};

                    template <class FSM,class Event>
                    void no_transition(Event const& ,FSM&, int ){}
                };
            }


                //! Unit event state enum definition used on WorkUnitManagment class method
            typedef enum UnitState {
                UnitStateUnpublished,
                UnitStateStartPublishing,
                UnitStatePublishing,
                UnitStatePublished,
                UnitStateStartUnpublishing,
                UnitStateUnpublishing,
                UnitStatePublishingFailure
            } UnitState;

                //! type definition for state machine
            typedef boost::msm::back::state_machine< work_unit_state_machine::wu_state_machine >	WorkUnitStateMachine;

                //! struct for the managment fo single control unit state machine
            /*!
             Every control unit has his individual state machine taht permit to regulate the
             process of publishing and online maintenance
             */
            class WorkUnitManagement {
                    //! registration pack used to send work unit descirption to mds
                chaos::common::data::CDataWrapper			mds_registration_message;
                    //! state machine for the control unit instance
                WorkUnitStateMachine	wu_instance_sm;

                bool    active;
                int     publishing_counter_delay;
                
                int sendConfPackToMDS(chaos::common::data::CDataWrapper& dataToSend);
                int sendLoadCompletionToMDS(const std::string& control_unit_uid);
                inline string getCurrentStateString();
            public:
                chaos::common::message::MDSMessageChannel   *mds_channel;
                    //! abstract contro unit isntalce
                ChaosSharedPtr<AbstractControlUnit>	work_unit_instance;
                
                    //! default constructor adn destructor
                WorkUnitManagement(ChaosSharedPtr<AbstractControlUnit>& _work_unit_instance);
                ~WorkUnitManagement();
                
                    //! return the state of the unit state machine
                UnitState getCurrentState();
                
                    //! turn on the control unit
                /*!
                 This message will activate the registration process of the control unit
                 and all the step are done to achive the registration.
                 */
                void turnOn();
                
                    //! turn off the control unit
                /*!
                 This message will start the unload process of the work unit.
                 if it is running, before that the process for the unpublish can 
                 start it will be stopped and deinitialized.
                 */
                void turnOFF();
                
                    //! manage the internal state machine
                void scheduleSM();
                
                    //! manage the ack pack
                bool manageACKPack(chaos::common::data::CDataWrapper& ack_pack);
                
                bool smNeedToSchedule();
                
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__WorkUnitManagement__) */
