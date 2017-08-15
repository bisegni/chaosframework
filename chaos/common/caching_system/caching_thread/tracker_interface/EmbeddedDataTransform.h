/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
