/*
 *	EmbeddedDataTransform.h
 *	!CHAOS
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
#ifndef CHAOSFramework_EmbeddedDataTransform_h
#define CHAOSFramework_EmbeddedDataTransform_h

#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>


namespace chaos {
    
    
    namespace caching_system {
        namespace caching_thread{
            template <class T, class U = T>
            class AbstractDeviceTracker;
        }
        template <typename T,typename D>
        
        /*!
         * This class represents an abstraction for a filter interface. In this context, 
         * a filter is an object that performs a mapping beetween two data structure (eventually the same )
         * and that can operate calcolation to fill values of target dta structure beginnnig from original data.
         * All values created form the filter will be cached for others process who need it. 
         * You can create your own filter just implementing this interface and passing it to TransformTracker. 
         */
        class EmbeddedDataTransform{
            
        friend class AbstractDeviceTracker;
            
        public:

            /*!
             * Implements this function to create your own filter. Data will be automacally passed to this function 
             * when they are available. The result will be cached for others requests.
             */
            virtual D* trasformData(T* data)=0;
            
            
        };
        
    }
}




#endif
