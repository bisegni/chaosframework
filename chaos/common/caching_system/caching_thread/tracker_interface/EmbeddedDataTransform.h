//
//  EmbeddedDataTransform.h
//  CHAOSFramework
//
//  Created by Flaminio Antonucci on 07/01/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_EmbeddedDataTransform_h
#define CHAOSFramework_EmbeddedDataTransform_h

#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>
#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>

namespace chaos {
    
    
    namespace caching_system {
        // template<typename T>
        //  class DeviceTracker;
        
        template <typename T,typename D>
        class EmbeddedDataTransform{
            
            /* template <typename TT>*/friend class AbstractDeviceTracker;
            
        public:
            //  DataTransformFilterInterface(){}
            
            virtual D* trasformData(T* data)=0;
            
            
        };
        
    }
}




#endif
