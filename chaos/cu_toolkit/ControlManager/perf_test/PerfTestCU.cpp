//
//  PerfTestCU.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 1/19/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/cu_toolkit/ControlManager/perf_test/PerfTestCU.h>

using namespace chaos;
using namespace chaos::perf_test;

#define WORK_SIMULATION_TIME 20000

PerfTestCU::PerfTestCU() {
    //genrate random unique name
    deviceName.assign("perf_");
    deviceName.append(UUIDUtil::generateUUID());
}

PerfTestCU::~PerfTestCU() {
    
}

/*
 Define the Control Unit Dataset and Actions
 */
void PerfTestCU::defineActionAndDataset(CDataWrapper&)throw(CException) {
    //set the default delay for the CU
    setDefaultScheduleDelay(20000);
    
    //add managed device di
    addDeviceId(deviceName.c_str());
    
    
    //setup the dataset
    addAttributeToDataSet(deviceName.c_str(),
                          "rt_average",
                          "Round trip average time",
                          DataType::TYPE_INT32,
                          DataType::Output);
    
    addAttributeToDataSet(deviceName.c_str(),
                          "seq_lost",
                          "Number of packet lost",
                          DataType::TYPE_INT32,
                          DataType::Output);
    
    addAttributeToDataSet(deviceName.c_str(),
                          "last_sequence",
                          "Current Sequence",
                          DataType::TYPE_INT64,
                          DataType::Output);
    
    addInputInt64AttributeToDataSet<PerfTestCU>(deviceName.c_str(),
                                                  "rt_seq",
                                                  "Input sequence for computing round trip time",
                                                  this,
                                                  &PerfTestCU::computeSequence);
    
    addInputInt64AttributeToDataSet<PerfTestCU>(deviceName.c_str(),
                                                "work_time",
                                                "time in milliseconds spent to simualte the work",
                                                this,
                                                &PerfTestCU::setWorkTime);
}


/*(Optional)
 Initialize the Control Unit and all driver, with received param from MetadataServer
 */
void PerfTestCU::init(CDataWrapper*) throw(CException) {
    lastSequence = 0;
    averageRoundTrip = 0;
    lostSequences = 0;
    lastSequenceComputed = 0;
    sequenceOverSample = 0;
}


/*
 Execute the work, this is called with a determinated delay, it must be as fast as possible
 */
void PerfTestCU::run(const string&) throw(CException) {
    lastSequence++;
    boost::chrono::microseconds msecDiff(0);
    
    sequenceTimestampedMap.insert(make_pair<int64_t, high_resolution_clock::time_point>(lastSequence, timeReferenceWorkSimulation = boost::chrono::steady_clock::now()));
    for (; msecDiff.count() >= WORK_SIMULATION_TIME;) {
        //simulate hard work
        msecDiff = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now() - timeReferenceWorkSimulation);
    }
    
    //prepare data for push
    CDataWrapper *acquiredData = getNewDataWrapperForKey(deviceName.c_str());
    acquiredData->addInt64Value("rt_average", averageRoundTrip/lastSequenceComputed);
    acquiredData->addInt64Value("seq_lost", lostSequences);
    acquiredData->addInt64Value("last_sequence", lastSequence);
    
    //push data on cache
    pushDataSetForKey(deviceName.c_str(), acquiredData);
}


/*
 The Control Unit will be stopped
 */
void PerfTestCU::stop(const string&) throw(CException) {
    
}


/*(Optional)
 The Control Unit will be deinitialized and disposed
 */
void PerfTestCU::deinit(const string&) throw(CException) {
    //remove all old data
    sequenceTimestampedMap.clear();
}

void PerfTestCU::computeSequence(const std::string& deviceID, const int64_t& sequence) {
    rtTimeTemp = boost::chrono::steady_clock::now();
    if(sequence == lastSequenceComputed) {
        sequenceOverSample++;
    } else {
        lostSequences += (sequence - lastSequenceComputed + 1);
    }
    
    
    boost::chrono::microseconds msecRoundTripTime = boost::chrono::duration_cast<boost::chrono::microseconds>(rtTimeTemp - sequenceTimestampedMap[sequence]);

    //computer
    averageRoundTrip += msecRoundTripTime.count();
    lastSequenceComputed++;
}

void PerfTestCU::setWorkTime(const std::string& deviceID, const int64_t& timeInMSec) {
    
}
