/*
 *	DataFetcherInterface.h
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
