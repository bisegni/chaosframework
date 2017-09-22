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

#ifndef CachingSystem_SmartPointer_h
#define CachingSystem_SmartPointer_h

#include <chaos/common/caching_system/common_buffer/Element.h>
#include <chaos/common/caching_system/common_buffer/data_element/AbstractDataElement.h>
//#include <MioElemento.h>
namespace chaos {
    
    
    namespace caching_system {
        
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
