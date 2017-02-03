/*
 *	ChaosAgent.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/02/2017 INFN, National Institute of Nuclear Physics
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

#include <ChaosAgent/ChaosAgent.h>
using namespace chaos::agent;

chaos::WaitSemaphore ChaosAgent::wait_close_semaphore;

ChaosAgent::ChaosAgent() {}

ChaosAgent::~ChaosAgent() {}

void ChaosAgent::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosAgent>::init(argc, argv);
}

void ChaosAgent::init(istringstream &initStringStream) throw (CException) {
    ChaosCommon<ChaosAgent>::init(initStringStream);
}

void ChaosAgent::init(void *init_data)  throw(CException) {
    ChaosCommon<ChaosAgent>::init(init_data);
    if (signal((int) SIGINT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-1, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGQUIT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-2, "Error registering SIGERR signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGTERM, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-3, "Error registering SIGTERM signal", __PRETTY_FUNCTION__);
    }
}

void ChaosAgent::start()throw(CException) {
    ChaosCommon<ChaosAgent>::start();
}

void ChaosAgent::stop()throw(CException) {
    ChaosCommon<ChaosAgent>::stop();
}

void ChaosAgent::deinit()throw(CException) {
    ChaosCommon<ChaosAgent>::deinit();
}

void ChaosAgent::signalHanlder(int signal_number) {
    wait_close_semaphore.unlock();
}
