//
//  PerfTestCU.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 1/19/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__PerfTestCU__
#define __CHAOSFramework__PerfTestCU__

#include <string>
#include <map>
#include <stdint.h>
#include <boost/chrono.hpp>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

using namespace std;
namespace chaos {
    namespace perf_test {
        
        /*!
         Control Unit managend by Control Manager used by MDS for network timing  performance test
         */
        class PerfTestCU : public AbstractControlUnit {
            string deviceName;
            int64_t averageRoundTrip;
            int64_t lostSequences;
            int64_t lastSequenceComputed;
            int64_t sequenceOverSample;
            int64_t lastSequence;
            map<int64_t, high_resolution_clock::time_point> sequenceTimestampedMap;
            
            TimingUtil tUtil;
            high_resolution_clock::time_point timeReferenceWorkSimulation;
            high_resolution_clock::time_point rtTimeTemp;
            PerfTestCU();
            ~PerfTestCU();
            
            inline void computeSequence(const std::string& deviceID, const int64_t& sequence);
            inline void setWorkTime(const std::string& deviceID, const int64_t& timeInMSec);
        protected:
            /*
             Define the Control Unit Dataset and Actions
             */
            virtual void defineActionAndDataset(CDataWrapper&)throw(CException);
            
            /*(Optional)
             Initialize the Control Unit and all driver, with received param from MetadataServer
             */
            void init(CDataWrapper*) throw(CException);
            
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
        };
        
    }
}

#endif /* defined(__CHAOSFramework__PerfTestCU__) */
