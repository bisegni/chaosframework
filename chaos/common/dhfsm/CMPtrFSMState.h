/*	
 *	CMPtrFSMState.h
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
#ifndef Common_CMPtrFSMState_h
#define Common_CMPtrFSMState_h

#include "FSMState.h"
#include <string>

namespace chaos {
    using namespace std;
    /*
     Template class that represent a state implemented by a method af a class
     */
    template <typename T>
    class CMPtrFSMState: public FSMState {
        typedef CDataWrapper* (T::*FSMStateMehtodPointerDef)(CDataWrapper*);
        
        string _stateName;
        T *_objectPtr;
        CDataWrapper *_inData;
        CDataWrapper *_outData;
        FSMStateMehtodPointerDef _methodOffset;

    public:
        /*
         Constructor
         */
        CMPtrFSMState(T* objectPtr, ActionPointerDef methodOffset, const char *stateName):name(stateName){
            _objectPtr    = objectPtr;
            _methodOffset = methodOffset;
            _inData  = NULL;
            _outData = NULL;
        }
        
        /*
         Destructor
         */
        ~CMPtrFSMState(){
            if(_inData) delete(_inData);
            if(_outData) delete(_outData);
        }
        
        /*
         Enter event, this is launched before the state must execute the work
         */
        virtual void onEnter(CDataWrapper *inData) {
            _inData = inData;
        }
        
        /*
         Work event, this is executed in a separate thread
         */
        virtual void executeState() {
            CHAOS_ASSERT(objectPtr && methodOffset)
            _outData =  ((*objectPtr).*methodOffset)(_inData);
        }
        
        /*
         Exit event, this is launched after the state has finished the work,
         the CDataWrapper can be filled with some data necassary for next state
         */
        virtual void onExit(CDataWrapper *outData) {
            if(!outData || !_outData) return;
                //add all data to the passed datawrapper
            outData->appendAllElement(*_outData);
        }
    };
}
#endif
