/*
 *	DataTransformFilterInterface.h
 *	!CHOAS
 *	Created by Antonucci Flaminio.
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

#ifndef CachingSystem_DataTransformFilterInterface_h
#define CachingSystem_DataTransformFilterInterface_h

#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>
#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>

namespace chaos {
    
    
    namespace caching_system {
        // template<typename T>
        //  class DeviceTracker;
        
        template <typename T>
        class DataTransformFilterInterface{
            
            /* template <typename TT>*/friend class AbstractDeviceTracker;
            
        public:
            //  DataTransformFilterInterface(){}
            
            virtual void trasformData(DataElement<T>* data)=0;
            
            
        };
        
    }
}


#endif
