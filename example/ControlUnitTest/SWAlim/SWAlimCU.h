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
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include "SWAlim.h"
#include "TcpChannel.h"
#include "ModbusProtocol.h"


using namespace std;
using namespace chaos;
using namespace boost;
using namespace boost::posix_time;


class SWAlimCU : public chaos::cu::AbstractControlUnit {
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
    virtual void defineActionAndDataset()throw(CException);
    
    /*(Optional)
     Initialize the Control Unit and all driver, with received param from MetadataServer
     */
    void init(const string&) throw(CException);
    
    /*
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
    void run(const string&) throw(CException);
    
    /*
     The Control Unit will be stopped
     */
    void stop(const string&) throw(CException);
    
    /*(Optional)
     The Control Unit will be deinitialized and disposed
     */
    void deinit(const string&) throw(CException);
    
    /*
        Test Action Handler
     */
    CDataWrapper* actionTestOne(CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    CDataWrapper* actionTestTwo(CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    CDataWrapper* resetStatistic(CDataWrapper*, bool&);

};

#endif
