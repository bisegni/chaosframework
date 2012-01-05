    //
    //  Common.h
    //  ControlSystemLib
    //
    //  Created by Bisegni Claudio on 19/12/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ControlSystemLib_ChaosCommon_h
#define ControlSystemLib_ChaosCommon_h

#include <chaos/common/configuration/GlobalConfiguration.h>

/*! \mainpage !Chaos - Control system based on a Highly Abstracted and Open Structure
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *  
 * etc...
 */

//! Default chaos namespace used to group all common api
namespace chaos {
   
    //! Chaos common engine class
    /*! 
        This is the base class for the other toolkit, it thake care to initialize all common
        resource used for the base chaos function
     */
    class ChaosCommon {
        //! Network address scan
        /*! 
         This methdo will take care for network address scan process
         */
        void scanForLocalNetworkAddress();
        
    protected:
        //! Constructor
        /*! 
          A more elaborate description of the destructor.
         */
        ChaosCommon();
        
        //! Destructor
        /*! 
         A more elaborate description of the destructor.
         */
        ~ChaosCommon();

        //! Init method
        /*! 
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        virtual void init(int argc = 1, const char* argv[] = NULL) throw(CException);
        
    public:
        //! Return the global configuration for the current process
        /*! 
         Return the GlobalConfiguration singleton instance
         */
        GlobalConfiguration *getGlobalConfigurationInstance();
    }; 
}


#endif
