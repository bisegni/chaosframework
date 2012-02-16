    //
    //  SetupStateManager.h
    //  CHAOSFramework
    //
    //  Created by Bisegni Claudio on 29/01/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

#ifndef CHAOSFramework_SetupStateManager_h
#define CHAOSFramework_SetupStateManager_h

#include <chaos/common/exception/CException.h>

#include <boost/thread/mutex.hpp>

namespace chaos {
    
    using namespace boost;
    
        //! Utility class for managing the setup state
    /*! \class SetupStateManager
     \brief
     
     */
    class SetupStateManager {
            //!Initialized flag
        int currentState;
        
            //!mutex for multithreading managment of sand box
        /*!
         The muthex is needed because the call to the action can occours in different thread
         */
        boost::mutex managing_init_deinit_mutex;
        
    public:
            //!Base Constructor
        /*! 
         Base initialization for internal state
         */
        SetupStateManager():currentState(0){}
        
            //!growup the internal state to newStateID
        /*! 
         the growing state can be only the next level of the currentState
         \param stateID new state requested
         \param errorMessageDomain error domain in case of innacessible state
         \param errorMessageText error message in case of inaccesible state
         */
        inline void levelUpFrom(int currentStateID, const char* const errorMessageText)  throw (CException) {
            boost::mutex::scoped_lock  lock(managing_init_deinit_mutex);
            if(currentStateID != currentState) throw CException(currentStateID, errorMessageText, "SetupStateManager::levelUp");
            
            currentState++;
        }
        
            //!set down the internal state to newStateID
        /*! 
         the set down state operation can be only if the previous level of the currentState
         \param stateID new state requested
         \param errorMessageDomain error domain in case of innacessible state
         \param errorMessageText error message in case of inaccesible state
         */
        inline void levelDownFrom(int currentStateID, const char* const errorMessageText)  throw (CException) {
            boost::mutex::scoped_lock  lock(managing_init_deinit_mutex);
            if(currentStateID != currentState) throw CException(currentStateID, errorMessageText, "SetupStateManager::levelDown");
            
            currentState--;
        }
    };
}
#endif
