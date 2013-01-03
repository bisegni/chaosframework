/*
 *	SmartPointer.h
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

#ifndef CachingSystem_SmartPointer_h
#define CachingSystem_SmartPointer_h

#include <chaos/common/caching_system/common_buffer/Element.h>
#include <chaos/common/caching_system/common_buffer/data_element/AbstractDataElement.h>
//#include <MioElemento.h>
namespace chaos {
    

namespace caching_system{
    
    template <typename T>
    class SmartPointer  {
        
    private:
        Element<AbstractDataElement<T>  >* elemento;
        
    public:
        
        SmartPointer(Element<AbstractDataElement<T>  >* e){
            elemento=NULL;
            
            e->incrementReferenceCount();
            this->elemento=e;
            
            
            
        }
        
        
        
        ~SmartPointer(){
            
            elemento->decrementReferenceCount();
            //delete elemento;
            
        }
      
        T *operator->(){
            //elemento->data->getData
            return elemento->data->getData();
        }
        
        T* getData(){
            
            return elemento->data->getData();
        }
        
        void incrementsReference(){
            this->elemento->incrementReferenceCount();
        }
        Element<AbstractDataElement<T>  >* getElement(){
            return this->elemento;
        }
        
       /* long getTimestamp(){
        
            return elemento->getTimestamp();
        }*/
        
      /*  T* getDataDriver(){
            return elemento->data->getData();
        
        }*/
    };
    
    
    }
    
}

#endif
