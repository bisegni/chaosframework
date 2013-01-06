/*
 *	IteratorBase.h
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

#ifndef CachingSystem_IteratorBase_h
#define CachingSystem_IteratorBase_h



#include <iostream>
#include <chaos/common/caching_system/common_buffer/Element.h>
#include <stdio.h>
#include <chaos/common/caching_system/common_buffer/SmartPointer.h>
#include <fstream>
#include <cstring>
#include <vector>

#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

namespace chaos {
    
    
    
    namespace caching_system {
        template<class V>
        class CommonBuffer;
        template <typename V>
        /*!
         * This class is used to access to queue for reading thread.
         * Also garbage collector should have this object to mark old elements.
         * \note with this object, you have read-only access to the queue.
         */
        class IteratorBase{
            friend class CommonBuffer<V>;
            
        private:
            uint32_t id;
        protected:
            
            Element<AbstractDataElement<V> >* current; //!< pointer to current element.
            CommonBuffer<V>* queue;//!< pointer to queue
            SmartPointer<V>* stopAndResume;//!< Smart pointer used to resume from a previous current if it is valid.
            Element<AbstractDataElement<V> >* lastDiv;
            
            IteratorBase(){
                
                
                
            }
            
            
            /*
             * Update current to next value and, if it is on pivot and readers count is zero,
             * switch pivot on next elements.
             */
            void  updateIterator(){
                // increments reader count of next element, becuase i don't wanto to miss it
                
                
                this->current->next->increment();
                
                // decrements reader count of acutal element, becuase i want to release it
                //  if(this->current->readerCount>0)
                if(this->current->readerCount>0)
                    this->current->decrementCounter();
                
                
                //and now, check if i have to update pivot
                
                
                // shift to the next one
                this->current=this->current->next;
                if(current==0){
                    
                    current->increment();
                }
                
                
                
            }
            
            
            /*!
             * Release the queue.
             * \note remember to call this function when you have finished reading, otherwise
             * no one element could be destroyed and the queue will grow indefinitely.
             */
            void leaveStream(){
                if(this->current->readerCount>0){
                    
                    this->current->decrementCounter();
                    
                    
                    
                    
                }else{
                    
                    //std::cout<<"Ramo non atteso\n";
                }
                
                
            }
            
            
            
            /*!
             * It sets the current element. Only common buffer shuld use this function.
             */
            void inline setPivot(Element<AbstractDataElement<V> >* pivot){
                this->current=pivot;
                lastDiv=NULL;
                
            }
            
            
            /*!
             * It swap current to next valid element.
             * It returns true is any, false otherwise.
             */
            bool inline jumpToNextValidElement(){
                //ora vai comunque all'ultimo valido
                Element<AbstractDataElement<V> >* testa=queue->head;
                
                if(current->readerCount==0)
                    current->increment();
                
                if(isReady()){
                    if(!current->isValid()){
                        do {
                            
                            
                            //here the current is out-of-date, so i have to swap on until i don't get
                            // an up-to-date element
                            
                            if(isReady()){
                                updateIterator();
                                
                            }
                            else{
                                //i have reached the head, and i didn't find any valid element.
                                // so i get a smart pointer to current and return false,
                                //waiting for better times
                                if(stopAndResume!=NULL){
                                    delete stopAndResume;
                                    stopAndResume=NULL;
                                    
                                }
                                stopAndResume=new SmartPointer<V>(current);
                                leaveStream();
                                
                                return false;
                            }
                        } while (!current->isValid()&&current!=testa);
                    }
                    
                    return true;
                }else{
                    if(stopAndResume!=NULL){
                        delete stopAndResume;
                        stopAndResume=NULL;
                        
                    }
                    stopAndResume=new SmartPointer<V>(current);
                    leaveStream();
                    return false;
                }
            }
            
        public:
            
            
            /*!
             * Create a new Iterator Object linked to a common buffer.
             * Use buffered reader to start reading.
             */
            IteratorBase(CommonBuffer<V>* queue){
                
                this->queue=queue;
                stopAndResume=NULL;
                this->current=NULL;
                lastDiv=NULL;
                
            }
            
            
            
            /*!
             * Release the the buffer in a safety way and free memory
             */
            ~IteratorBase(){
                /*if(this->current!=NULL)
                 leaveStream();
                 */
                if(stopAndResume!=NULL)
                    delete stopAndResume;
            }
            
            /*!
             * It returns true is the object is ready.
             */
            bool inline isReady(){
                Element<AbstractDataElement<V> >* testa=this->queue->head;
                if(this->current->next==NULL||this->current==testa||current->data==NULL){
                    return false;
                }else{
                    return true;
                }
            }
            
            
            int getId(){
                return this->id;
            }
            
            void setId(int id){
                this->id=id;
            }
            
            long getCounter(){
                return queue->getCounter();
            }
            
            long getValidityWindowSize(){
                return queue->getElementInValidity();
            }
            
        };
        
        
        
    }
}

#endif
