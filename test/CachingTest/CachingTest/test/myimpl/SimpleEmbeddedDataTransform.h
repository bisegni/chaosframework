//
//  SimpleEmbeddedDataTransform.h
//  CachingTest
//
//  Created by Flaminio Antonucci on 08/01/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#ifndef CachingTest_SimpleEmbeddedDataTransform_h
#define CachingTest_SimpleEmbeddedDataTransform_h

#include "chaos/common/caching_system/caching_thread/tracker_interface/EmbeddedDataTransform.h"
#include "MioElemento.h"
#include "FilteredMagnet.h"
using namespace chaos;
class SimpleDataTransform : public caching_system::EmbeddedDataTransform<Magnete,FilteredMagnet>{
    
    
public:
    FilteredMagnet* trasformData(Magnete* data){
        
        FilteredMagnet* filtered= new FilteredMagnet(*data->getTop(),*data->getDown(),0);
        return filtered;
        
    }
    
    
    
};

#endif
