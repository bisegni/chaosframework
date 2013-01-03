//
//  SimpleDataAggragator.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 15/11/12.
//
//

#ifndef CachingSystem_SimpleDataAggragator_h
#define CachingSystem_SimpleDataAggragator_h
#include "MioElemento.h"
#include "DataAggregate.h"
#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataAggregationInterface.h>
using namespace caching_system;

class SimpleDataAggregator : public DataAggregationFilterInterface<Magnete,DataAggregate> {
    
public:
    SimpleDataAggregator(){
    
    }
    
    
    DataAggregate* aggregateData(std::vector<DataElement<Magnete>* >* data){
        
        double canale1=0;
        double canale2=0;
        double canale3=0;
        double canale4=0;
    
        for(int i=0;i<data->size();i++){
            Magnete* magnetino=data->at(i)->getData();
            canale1+= (((long)(magnetino->getTop()))%17);
            canale2+=((long)(magnetino->getDown())%17);
            canale3+=((long)(magnetino->getLeft())%17);
            canale4+=((long)(magnetino->getRight())%17);
        
        }
        
       DataAggregate* aggragazione=new  DataAggregate(canale1,canale2,canale3,canale4);
        
        return aggragazione;
    
    }

    
    
};

#endif
