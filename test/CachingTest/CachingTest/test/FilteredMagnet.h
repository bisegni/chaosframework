//
//  FilteredMagnet.h
//  CachingTest
//
//  Created by Flaminio Antonucci on 08/01/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#ifndef CachingTest_FilteredMagnet_h
#define CachingTest_FilteredMagnet_h


class FilteredMagnet {
    double canale1;
    double canale2;
    uint64_t timestamp;
public:
    FilteredMagnet(double canale1,double canale2,uint64_t timestamp){
        
        this->canale1=canale1;
        this->canale2=canale2;
    
    }
    
    
    friend void operator<<(std::ostream& out,const FilteredMagnet& mag){
        out<<"canale1: "<<mag.canale1<<"; ";
        out<<"canale2: "<<mag.canale2<<"; ";
      
    }

    
};
#endif
