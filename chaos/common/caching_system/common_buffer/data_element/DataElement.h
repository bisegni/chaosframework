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
