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
#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>

namespace chaos {
    
    
    namespace caching_system {
        // template<typename T>
        //  class DeviceTracker;
        
        template <typename T>
        class DataFetcherInterface{
            
            /*template <typename TT>*/friend class AbstractDeviceTracker;
            
        public:
            // DataFetcherInterface(){}
            CommonBuffer<T>* buffer;
            
            DataElement<T>* _getData() {
                T* data=new T();
                uint64_t timestamp;
                getData(data,&timestamp);
                //std::cout<<"tempo:"<<timestamp;
                //  std::cout<<*data;
                DataElement<T>* ritorno= new DataElement<T>(data,timestamp);
                return ritorno;
                
            }
            
            void setBuffer(CommonBuffer<T>* buffer){
                this->buffer=buffer;
            }
            
            CommonBuffer<T>* getBuffer(){
                return this->buffer;
            }
            
            
            virtual void getData(T *newData, uint64_t* ts)=0;
        };
        
    }
}
#endif
