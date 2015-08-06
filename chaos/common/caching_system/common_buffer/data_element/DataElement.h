/*
 *	DataElement.h
 *	!CHAOS
 *	Created by Flaminio Antonucci.
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

#ifndef CachingSystem_DataDriver_h
#define CachingSystem_DataDriver_h

#include <chaos/common/caching_system/common_buffer/data_element/AbstractDataElement.h>
namespace chaos {
    
    namespace caching_system {
        template <typename T>
        class DataElement  : public caching_system::AbstractDataElement<T>{
            long timeStamp;
            T* payload;
            
        public:
            
            DataElement(){
                
                
            }
            DataElement(T* payload,long timeStamp){
                this->timeStamp=timeStamp;
                this->payload=payload;
                
            }
            
            ~DataElement(){
                delete payload;
            }
            
            T* getData(){
                return payload;
            }
            
            long getTimestamp(){
                
                return this->timeStamp;
            }
            
            void setData(T* payload,long timeStamp){
                this->timeStamp=timeStamp;
                this->payload=payload;
                
            }
            
            
            
        };
    }
}
#endif
