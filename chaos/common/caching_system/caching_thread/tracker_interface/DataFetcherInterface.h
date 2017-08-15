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

#ifndef CachingSystem_DataFetcherInterface_h
#define CachingSystem_DataFetcherInterface_h
#include <chaos/common/caching_system/common_buffer/data_element/DataElement.h>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>

namespace chaos {
    
    
    namespace caching_system {
        
        class AbstractDeviceTracker;
        
        template <typename T>
        
        
        /*!
         * This class represents abstraction for feetching data. It is used from a tracker to perform retrieving of
         * data . So tracker can be decoupled from the logic of how effectively data have to be retrieved. To create
         * your particular driver, you need to inherit this class and implements :
         * virtual void getData(T& newData, uint64_t& ts)=0;
         *
         */
        class DataFetcherInterface{
            
            friend class AbstractDeviceTracker;
            
        public:
            CommonBuffer<T>* buffer;
            
            /*!
             * Template method uesd to wrap two information needed (timestamp and data) in
             * a DataElement object.
             */
            DataElement<T>* _getData() {
                uint64_t timestamp;
                T *newData = new T();
                getData(*newData, timestamp);
                return new DataElement<T>(newData,timestamp);
                
            }
            
            
            void setBuffer(CommonBuffer<T>* buffer){
                this->buffer=buffer;
            }
            
            CommonBuffer<T>* getBuffer(){
                return this->buffer;
            }
            
            /*!
             * Implements this function to serve the needed informations.
             */
            virtual void getData(T& newData, uint64_t& ts)=0;
        };
        
    }
}
#endif
