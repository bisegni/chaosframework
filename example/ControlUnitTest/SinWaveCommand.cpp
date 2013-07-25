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
#include "SinWaveCommand.h"
#include <boost/lexical_cast.hpp>

using namespace chaos;
namespace cccs = chaos::cu::control_manager::slow_command;

SinWaveCommand::SinWaveCommand():rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred) {
    
}

SinWaveCommand::~SinWaveCommand() {
    
}

// return the implemented handler
uint8_t SinWaveCommand::implementedHandler() {
    return  cccs::HandlerType::HT_Set |
            cccs::HandlerType::HT_Acquisition |
            cccs::HandlerType::HT_Correlation;
}

// Start the command execution
void SinWaveCommand::setHandler(chaos::CDataWrapper *data) {
   // chaos::cu::DeviceSchemaDB *deviceDB = NULL;
    
    srand((unsigned)time(0));
    PI = acos((long double) -1);
    sinevalue = NULL;
    
    
    pointSetting = getValueSetting((cccs::AttributeIndexType)0);
    points = pointSetting->getCurrentValue<uint32_t>();
  /*  if(data == NULL) {
        //we are at default submition of the command
        deviceDB = getDeviceDatabase();
        if(deviceDB) {
            RangeValueInfo attributeInfo;
            std::string pName = "points";
            uint32_t defaultPointValue = 30;
            deviceDB->getAttributeRangeValueInfo(pName, attributeInfo);
            if(attributeInfo.defaultValue.size()) {
                defaultPointValue = boost::lexical_cast<uint32_t>(attributeInfo.defaultValue);
            }
            pointSetting->setNextValue(&defaultPointValue, sizeof(uint32_t));
        }
    }*/
    setWavePoint();

    *(freq = getValueSetting((cccs::AttributeIndexType)1)->getCurrentValue<double>()) = 1.0;
    *(bias = getValueSetting((cccs::AttributeIndexType)2)->getCurrentValue<double>()) = 5.0;
    *(phase = getValueSetting((cccs::AttributeIndexType)3)->getCurrentValue<double>()) = 0.0;
    *(gain = getValueSetting((cccs::AttributeIndexType)4)->getCurrentValue<double>()) = 0.0;
    *(gainNoise = getValueSetting((cccs::AttributeIndexType)5)->getCurrentValue<double>()) = 0.5;
    
   /*
    freq = 1.0;
    gain = 5.0;
    phase = 0.0;
    bias = 0.0;
    gainNoise = 0.5;*/
    LAPP_ << "SinWaveCommand::setHandler";
}

// Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void SinWaveCommand::acquireHandler() {
    //chech if some parameter has changed
    changedIndex.clear();
    getChangedAttributeIndex(changedIndex);
    if(changedIndex.size()) {
        LAPP_ << "We have " << changedIndex.size() << " changed attribute";
        for (int idx =0; idx < changedIndex.size(); idx++) {
            cccs::ValueSetting *vSet = getValueSetting(changedIndex[idx]);
            
            //the index is correlated to the creation sequence so
            //the index 0 is the first input parameter "frequency"
            switch (vSet->index) {
                case 0://points
                    // apply the modification
                    setWavePoint();
                    break;
                    
                case 1:// frequency
                   
                    break;
                    
                case 2:// bias
                    
                    break;
                    
                case 3:// phase
                    break;
                    
                case 4:// gain
                    
                    break;
                case 5:// gain_noise
                    
                    break;
            }
        }
        
    }
    CDataWrapper *acquiredData = getNewDataWrapper();
    if(!acquiredData) return;
    
    //put the messageID for test the lost of package
    acquiredData->addInt32Value("id", ++messageID);
    computeWave(acquiredData);

    //submit acquired data
    pushDataSet(acquiredData);
    
    SL_STACK_RUNNIG_STATE;
}

// Correlation and commit phase
void SinWaveCommand::ccHandler() {
    SL_STACK_RUNNIG_STATE;
}

void SinWaveCommand::computeWave(CDataWrapper *acquiredData) {
    if(sinevalue == NULL) return;
    double interval = (2*PI)/(*points);
    boost::mutex::scoped_lock lock(pointChangeMutex);
    for(int i=0; i<(*points); i++){
        sinevalue[i] = ((*gain)*sin((interval*i)+(*phase))+(((double)randInt()/(double)100)*(*gainNoise))+(*bias));
    }
    acquiredData->addBinaryValue("sinWave", (char*)sinevalue, (int32_t)sizeof(double)*(*points));
}

/*
 */
void SinWaveCommand::setWavePoint() {
    boost::mutex::scoped_lock lock(pointChangeMutex);
    uint32_t tmpNOP = *pointSetting->getNextValue<uint32_t>();
    if(tmpNOP < 1) tmpNOP = 0;
    
    if(!tmpNOP){
        if(sinevalue){
            free(sinevalue);
            sinevalue = NULL;
        }
    }else{
        size_t byteSize = sizeof(double) * tmpNOP;
        double* tmpPtr = (double*)realloc(sinevalue, byteSize);
        if(tmpPtr) {
            sinevalue = tmpPtr;
            memset(sinevalue, 0, byteSize);
        }else{
            pointSetting->completedWithError();
        }
    }
    //notify that someone take care to manage the modification
    pointSetting->completed();
}