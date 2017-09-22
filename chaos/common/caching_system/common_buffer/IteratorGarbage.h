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

#ifndef CachingSystem_IteratorGarbage_h
#define CachingSystem_IteratorGarbage_h
#include <chaos/common/caching_system/common_buffer/IteratorBase.h>

namespace chaos {
    
    namespace caching_system {
        
        template <typename T>
        class IteratorGarbage : public IteratorBase<T> {
            friend class CommonBuffer<T>;
            
            
        protected:
            
            /*!
             * It creates a new iterator for the stream.
             * this constructur can be called safely only by common buffer,
             * so do not use it. Use IteratorReader(queue) instead of this.
             */
            IteratorGarbage(CommonBuffer<T>* queue,Element<AbstractDataElement<T> >* pivot){
                
                this->queue=queue;
                this->current=pivot;
                // current->incrementReferenceCount();
                this->stopAndResume=NULL;
                this->lastDiv=NULL;
                
                
            }
            
            
            /*!
             * Starting by pivot, it marks unused object if out-of-date, if any.
             * Use isReady() before to avoid bad memory access
             */
            bool markOldData(){
                
                
                Element<AbstractDataElement<T> >* tempDiv=this->queue->pivot;
                Element<AbstractDataElement<T> >* head=this->queue->head;
                
                Element<AbstractDataElement<T> >* next;
                
                
                
                if((!tempDiv->isValid())&&tempDiv->next!=NULL&&tempDiv!=head&&tempDiv->next!=head){
                    //if i am on pivot, and it is not valid, decrement reader count on it
                    if(this->lastDiv!=tempDiv){
                        this->lastDiv=tempDiv;
                        next=tempDiv->next;
                        //first i increment next os no one can delete it
                        next->increment();
                        //now decrement acutl object
                        
                        //   if(tempDiv->readerCount>0){
                        tempDiv->decrementCounter();
                        this->current=next;
                        
                        
                    }
                    
                    // }
                    
                    if(tempDiv->readerCount<=0){
                        
                        //update pivot if no one have done it
                        
                        this->queue->compareAndSwap(&(this->queue->pivot),tempDiv,tempDiv->next);
                        
                        
                        return true;
                        
                    }
                    
                    //if the counter is ont to zero, it means that someone is reading it.
                    //so pivot will be switched by him.
                    
                }
                
                return false;
                
                
                
            }
            
            
        public:
            
            
            
            
            
            
            void clearQueue(){
                
                
                
                
                this->queue->clearUnusedData();
                
                if(this->isReady()){
                    while(this->markOldData()){}
                }
                
                
                
            }
            
            
            
        };
        
    }
    
    
}
#endif
