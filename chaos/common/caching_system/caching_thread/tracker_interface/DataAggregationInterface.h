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

#ifndef CachingSystem_DataAggregationInterface_h
#define CachingSystem_DataAggregationInterface_h

#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>

namespace chaos {
    
    namespace caching_system {
        // template<typename T>
        //  class DeviceTracker;
        template <class T, class T2>
        //template <typename T>
        class DataAggregationFilterInterface{
            
            //  template <typename TT>friend class DeviceTracker;
            
        public:
            //DataAggregationFilterInterface(){}
            
            /*copy your data, vector  will be destroyed after this call*/
            virtual T2* aggregateData(std::vector<DataElement<T>* >* data)=0;
            
            
        };
        
    }
    
}





#endif
