//
//  WorkerCU.h
//  ChaosFramework
//
//  Created by bisegni on 23/06/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_WorkerCU_h
#define ChaosFramework_WorkerCU_h

#include <string>
#include <boost/random.hpp>
#include <boost/chrono.hpp>

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

using namespace std;
using namespace chaos;
using namespace boost;
using namespace boost::posix_time;

class WorkerCU : public AbstractControlUnit {
    typedef boost::mt19937 RNGType; 
    RNGType rng;
    uniform_int<> one_to_six;    
    variate_generator< RNGType, uniform_int<> > randInt; 
    bool writeRead;
    int64_t numberOfResponse;
    high_resolution_clock::time_point lastExecutionTime;
    high_resolution_clock::time_point currentExecutionTime;
    string _deviceID;
public:
    /*
     Construct a new CU with an identifier
     */
    WorkerCU();
    /*
     Construct a new CU with an identifier
     */
    WorkerCU(string&);
    /*
     Destructor a new CU with an identifier
     */
    ~WorkerCU();
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
    
    /* (Optional)
     Event from metadataserver or gui for update configuration
     */
    CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);
    
    /* (Optional)
     This event is received when a dataset attribute is request to be valorized
     */
    CDataWrapper* setDatasetAttribute(CDataWrapper*, bool&) throw (CException);
    
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
