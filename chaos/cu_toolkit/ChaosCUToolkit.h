/*	
 *	ChaosCUToolkit.h
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

#ifndef ChaosCUToolkit_H
#define ChaosCUToolkit_H
    //#pragma GCC visibility push(hidden)

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/SetupStateManager.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>


namespace chaos{
    /*! \page page_cut The Control Unit Toolkit
     *  \section page_cut_sec This toolkit represent the chaos driver for the real hardware to control
     *  
     */
    
        //! Chaos Contorl Unit Framework Master Class
    /*! 
     This class is a Singleton that need to be used to setup environment,
     add Custom Control unit, and start all Contro Unit environment
     */
    class ChaosCUToolkit : public ChaosCommon<ChaosCUToolkit>, public ServerDelegator, public SetupStateManager {
        friend class Singleton<ChaosCUToolkit>;
            //static boost::mutex monitor;
            //static boost::condition endWaithCondition;
        
        static WaitSemaphore waitCloseSemaphore;
        
        ChaosCUToolkit(){};
        ~ChaosCUToolkit(){};
        static void signalHanlder(int);
        
    public:
        typedef boost::mutex::scoped_lock lock;
        void init(int argc = 1, char* argv[] = NULL)  throw(CException);
        void start(bool waithUntilEnd=true, bool deinitiOnEnd=true);
        void stop();
        void deinit();
        void addControlUnit(AbstractControlUnit*);
    };
}
    //#pragma GCC visibility pop
#endif
