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
#ifndef ChaosFramework_BenchTestCU_h
#define ChaosFramework_BenchTestCU_h

#include "Batch_Reactor.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <chaos/common/utility/Atomic.h>
#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>

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
