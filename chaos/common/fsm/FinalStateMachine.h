//
//  FinalStateMachine.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/29/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__FinalStateMachine__
#define __CHAOSFramework__FinalStateMachine__

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>
namespace chaos {
    
    namespace fsm {
        BOOST_MSM_EUML_EVENT(play)
        
        // front-end: define the FSM structure
        struct id_states_machine : public boost::msm::front::state_machine_def<id_states_machine> {
            
            
            // The list of FSM states
            struct Deinitilized : public boost::msm::front::state<>
            {
                // every (optional) entry/exit methods get the event passed.
                template <class Event,class FSM>
                void on_entry(Event const&,FSM& ) {std::cout << "entering: Deinitilized" << std::endl;}
                template <class Event,class FSM>
                void on_exit(Event const&,FSM& ) {std::cout << "leaving: Deinitilized" << std::endl;}
                struct internal_guard_fct
                {
                    template <class EVT,class FSM,class SourceState,class TargetState>
                    bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
                    {
                        std::cout << "Deinitilized::internal_transition_table guard\n";
                        return false;
                    }
                };
                struct internal_action_fct
                {
                    template <class EVT,class FSM,class SourceState,class TargetState>
                    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
                    {
                        std::cout << "Deinitilized::internal_transition_table action" << std::endl;
                    }
                };
                // Transition table for Empty
                struct internal_transition_table : boost::mpl::vector< Internal < EventType::initialize, internal_action_fct ,internal_guard_fct> > {};
            };
            
            struct Initialized : public boost::msm::front::state<>
            {
                template <class Event,class FSM>
                void on_entry(Event const& ,FSM&) {std::cout << "entering: Initialized" << std::endl;}
                template <class Event,class FSM>
                void on_exit(Event const&,FSM& ) {std::cout << "leaving: Initialized" << std::endl;}
            };
            
            // the initial state of the player SM. Must be defined
            typedef Deinitilized initial_state;
            
            typedef boost::msm::front::Row <  Deinitilized   ,  EventType::initialize  , Initialized, boost::msm::front::none , boost::msm::front::none > deinit_init_row;
            typedef boost::msm::front::Row <  Initialized   ,  EventType::deinitialize  , Deinitilized, boost::msm::front::none , boost::msm::front::none > init_deinit_row;
            
            // Transition table for initialization services
            struct transition_table : boost::mpl::vector<
            deinit_init_row,
            init_deinit_row > {};
        };
    }
}

#endif /* defined(__CHAOSFramework__FinalStateMachine__) */
