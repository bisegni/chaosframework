//
//  SimpleDataFetcher.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 15/11/12.
//
//

#ifndef CachingSystem_SimpleDataFetcher_h
#define CachingSystem_SimpleDataFetcher_h
#include "MioElemento.h"
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>
#include <chaos/common/caching_system/common_buffer/helper/MillisTime.h>
#include <iostream>
using namespace chaos;
class SimpleDataFetcher : public chaos::caching_system::DataFetcherInterface<Magnete> {
    
private:
    double val;
    
   
    
public:
    SimpleDataFetcher(){
        val=0;
    }
    
    
    void getData(Magnete *newData, uint64_t* ts){
       // val=4;
        *ts = chaos::my_time::getMillisTimestamp();
        *newData=Magnete(val,val+1,val+2,val+3,*ts);
        val++;
       // val=((int)val)%13;
    
    }
};


#endif
