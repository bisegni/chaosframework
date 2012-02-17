/*	
 *	FSMState.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
