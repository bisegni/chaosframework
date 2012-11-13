/*	
 *	ChaosNodeDirectory.cpp
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
#include "ChaosNodeDirectory.h"
#include <csignal>
#include <chaos/common/exception/CException.h>

using namespace std;
using namespace chaos;
using namespace chaos::nd;
using boost::shared_ptr;

WaitSemaphore ChaosNodeDirectory::waitCloseSemaphore;

#define LCND_ LAPP_ << "[ChaosNodeDirectory]- "

    //! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosNodeDirectory::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosNodeDirectory>::init(argc, argv);
}
    //!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosNodeDirectory::init(istringstream &initStringStream) throw (CException) {
    ChaosCommon<ChaosNodeDirectory>::init(initStringStream);
}

/*
 *
 */
void ChaosNodeDirectory::init()  throw(CException) {
    SetupStateManager::levelUpFrom(0, "ChaosNodeDirectory already initialized");
    try {
        
        LCND_ << "Initializing";
        ChaosCommon<ChaosNodeDirectory>::init();
        if (signal((int) SIGINT, ChaosNodeDirectory::signalHanlder) == SIG_ERR) {
            throw CException(0, "Error registering SIGINT signal", "ChaosNodeDirectory::init");
        }
 
        if (signal((int) SIGQUIT, ChaosNodeDirectory::signalHanlder) == SIG_ERR) {
            throw CException(0, "Error registering SIG_ERR signal", "ChaosNodeDirectory::init");
        }

    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
        //start data manager
}

/*
 *
 */ 
void ChaosNodeDirectory::start(bool waithUntilEnd, bool deinitiOnEnd){
        //lock o monitor for waith the end
    SetupStateManager::levelUpFrom(1, "ChaosNodeDirectory already started");
    try {
        LCND_ << "Starting";

        

        LCND_ << "Started";
        //at this point i must with for end signal
        if(waithUntilEnd)
            waitCloseSemaphore.wait();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
        //execute the deinitialization of CU
    if(waithUntilEnd && deinitiOnEnd)deinit();
}

/*
 Stop the toolkit execution
 */
void ChaosNodeDirectory::stop() {
    SetupStateManager::levelDownFrom(2, "ChaosNodeDirectory already stoped");
    //lock lk(monitor);
        //unlock the condition for end start method
    //endWaithCondition.notify_one();
    waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosNodeDirectory::deinit() {
    LCND_ << "Stopping";
    SetupStateManager::levelDownFrom(1, "ChaosNodeDirectory already deinitialized");
        //start Control Manager
      
    LCND_ << "Stopped";
}

/*
 *
 */
void ChaosNodeDirectory::signalHanlder(int signalNumber) {
    ChaosNodeDirectory::getInstance()->levelDownFrom(2, "ChaosNodeDirectory already stoped");
    //lock lk(monitor);
        //unlock the condition for end start method
    //endWaithCondition.notify_one();
     waitCloseSemaphore.unlock();
}
