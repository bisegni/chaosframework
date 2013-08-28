/*	
 *	WorkerCU.h
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
#ifndef ChaosFramework_WorkerCU_h
#define ChaosFramework_WorkerCU_h

#include <string>
#include <boost/random.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <chaos/common/utility/Atomic.h>
#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>
#include "SWAlim.h"
#include "TcpChannel.h"
#include "ModbusProtocol.h"


using namespace std;
using namespace chaos;
using namespace boost;
using namespace boost::posix_time;

namespace c_data = chaos::common::data;

class SWAlimCU : public chaos::cu::RTAbstractControlUnit {
    typedef boost::mt19937 RNGType; 
    RNGType rng;
    uniform_int<> one_to_hundred;    
    variate_generator< RNGType, uniform_int<> > randInt; 
    double numberOfResponse;
    high_resolution_clock::time_point initTime;
    high_resolution_clock::time_point lastExecutionTime;
    high_resolution_clock::time_point currentExecutionTime;
    string _deviceID;
    
    TcpChannel* tcpChan;
    ModbusProtocol* prot;
    SWAlim* myalim;
    unsigned messageID;
public:
    /*
     Construct a new CU with an identifier
     */
    SWAlimCU();
    /*
     Construct a new CU with an identifier
     */
    SWAlimCU(string&);
    /*
     Destructor a new CU with an identifier
     */
    ~SWAlimCU();
    
    inline void setCurrent(const std::string& deviceID, const int32_t& current);
protected:
    /*
     Define the Control Unit Dataset and Actions
     */
    void unitDefineActionAndDataset()throw(CException);
    
	void unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver);
	
    // from abstract class
    void unitInit() throw(CException);
    
    // from abstract class
    void unitStart() throw(CException);
    
    // from abstract class
    void unitRun() throw(CException);
    
    // from abstract class
    void unitStop() throw(CException);
    
    // from abstract class
    void unitDeinit() throw(CException);
    
    /*
        Test Action Handler
     */
    c_data::CDataWrapper* actionTestOne(c_data::CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    c_data::CDataWrapper* actionTestTwo(c_data::CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    c_data::CDataWrapper* resetStatistic(c_data::CDataWrapper*, bool&);

};

#endif
