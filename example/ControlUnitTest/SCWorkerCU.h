/*
 *	SCWorkerCU.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __ControlUnitTest__SCWorkerCU__
#define __ControlUnitTest__SCWorkerCU__

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>

using namespace std;
using namespace chaos;
using namespace boost;
using namespace boost::posix_time;

namespace cu_driver = chaos::cu::driver_manager::driver;

class SCWorkerCU : public chaos::cu::SCAbstractControlUnit {
    string _deviceID;
protected:
    /*
     Define the Control Unit Dataset and Actions
     */
    void unitDefineActionAndDataset()throw(CException);
    void defineSharedVariable();
    void unitDefineDriver(std::vector<cu_driver::DrvRequestInfo>& neededDriver);
    /*(Optional)
     Initialize the Control Unit and all driver, with received param from MetadataServer
     */
    void unitInit() throw(CException);
    /*(Optional)
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
    void unitStart() throw(CException);
    /*(Optional)
     The Control Unit will be stopped
     */
    void unitStop() throw(CException);
    /*(Optional)
     The Control Unit will be deinitialized and disposed
     */
    void unitDeinit() throw(CException);
public:
    /*
     Construct a new CU with an identifier
     */
    SCWorkerCU(string&);
	
	/*
     Base destructor
     */
	~SCWorkerCU();
};


#endif /* defined(__ControlUnitTest__SCWorkerCU__) */
