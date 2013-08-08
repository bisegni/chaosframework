/*	
 *	BenchTestCU.h
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
#ifndef ChaosFramework_BenchTestCU_h
#define ChaosFramework_BenchTestCU_h

#include "Batch_Reactor.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <chaos/common/utility/Atomic.h>
#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>

using namespace std;
using namespace chaos;
using namespace boost;

class BenchTestCU : public chaos::cu::RTAbstractControlUnit {
    string reactorName;
    Batch_Reactor *reactorInstance;
    boost::shared_mutex _setControlValueMutext;
    high_resolution_clock::time_point lastExecutionTime;
    high_resolution_clock::time_point currentExecutionTime;
    int cycleCount;
public:

    /*
     Construct a new CU with an identifier
     */
    BenchTestCU(string& rName, Batch_Reactor *rInstance);
    /*
     Destructor a new CU with an identifier
     */
    ~BenchTestCU();

    inline void setControlA(const std::string& deviceID, const double& dValue);
    inline void setControlB(const std::string& deviceID, const double& dValue);
    inline void perturbateState(const std::string& deviceID, const double& dValue);
protected:
    /*
     Define the Control Unit Dataset and Actions
     */
    void unitDefineActionAndDataset()throw(CException);
    
	void unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver);
	
    //From base class
    void unitInit() throw(CException);
    
    //From base class
    void unitStart() throw(CException);
    
    //From base class
    void unitRun() throw(CException);
    
    //From base class
    void unitStop() throw(CException);
    
    //From base class
    void unitDeinit() throw(CException);
};

#endif
