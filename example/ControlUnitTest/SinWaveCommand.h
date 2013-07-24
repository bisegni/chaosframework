/*
 *	SinWaveCommand.h
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

#ifndef __ControlUnitTest__SinWaveCommand__
#define __ControlUnitTest__SinWaveCommand__

#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>

using namespace chaos;
namespace cccs = chaos::cu::control_manager::slow_command;

class SinWaveCommand : public cccs::SlowCommand {
    typedef boost::mt19937 RNGType;
    RNGType rng;
    uniform_int<> one_to_hundred;
    variate_generator< RNGType, uniform_int<> > randInt;
    double *sinevalue;
    uint32_t *points;
    cccs::ValueSetting *pointSetting;
    double *freq;
    double *gain;
    double *phase;
    double *bias;
    double *gainNoise;

    long double PI;
    int32_t messageID;
    boost::mutex pointChangeMutex;
    std::vector<cccs::AttributeIndexType> changedIndex;
    
    inline void computeWave(CDataWrapper *acquiredData);
    inline void setWavePoint();
protected:
    // return the implemented handler
    uint8_t implementedHandler();
    
    // Start the command execution
    void setHandler(chaos::CDataWrapper *data);
    
    // Aquire the necessary data for the command
    /*!
     The acquire handler has the purpose to get all necessary data need the by CC handler.
     \return the mask for the runnign state
     */
    void acquireHandler();
    
    // Correlation and commit phase
    void ccHandler();
    
public:
    SinWaveCommand();
    ~SinWaveCommand();
};

#endif /* defined(__ControlUnitTest__SinWaveCommand__) */
