//
//  FSMState.h
//  ControlSystemLib
//
//  Created by Bisegni Claudio on 01/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef Common_FSMState_h
#define Common_FSMState_h

#include <chaos/common/data/CDataWrapper.h>

#include <string>
namespace chaos {
    /*
     This class represent the base class of a state. A single state irepesent a single
     operation. Many operation can be executed in parallel thread by the state machine.
     */
    public FSMState {
    protected:
        string name;
    public:
        
        /*
         Enter event, this is launched before the state must execute the work
         */
        virtual void onEnter(CDataWrapper*) = 0;
        
        /*
         Work event, this is executed in a separate thread
         */
        virtual void executeState() = 0;
        
        /*
         Exit event, this is launched after the state has finished the work,
         the CDataWrapper can be filled with some data necassary for next state
         */
        virtual void onExit(CDataWrapper*) = 0;
        
        /*
         Return the state name
         */
        string& getName() {
            return name;
        }
        
        /*
         Set the name
         */
        void setName(const char * _name){
            name = _name;
        }
        /*
         Set the name
         */
        void setName(string& _name){
            name = _name;
        }
    }
}

#endif
