/*
 *	IteratorReader.h
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
#ifndef CachingSystem_IteratorReader_h
#define CachingSystem_IteratorReader_h


#include <iostream>
#include <chaos/common/caching_system/common_buffer/Element.h>
#include <chaos/common/caching_system/common_buffer/IteratorBase.h>
#include <stdio.h>
#include <chaos/common/caching_system/common_buffer/SmartPointer.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <chaos/common/caching_system/common_buffer/data_element/AbstractDataElement.h>

namespace chaos {

namespace caching_system {
   /* template <class T, class U>
    class AbstractDeviceTracker;*/

    
    template <typename V>
    /*!
     * This class is used to access to queue for reading thread.
     * Also garbage collector should have this object to mark old elements.
     * \note with this object, you have read-only access to the queue.
     */
    
    
    class IteratorReader : public IteratorBase<V>{
       // friend class AbstractDeviceTracker<V,V>;
    protected:
        bool chooseCurrent(){
            Element<AbstractDataElement<V> >* testa=this->queue->head;
            int pos=0;
            
            if(this->stopAndResume!=NULL){
                if(/*current!=NULL && */(this->current->isValid())){
                    this->current->increment();
                    //ho ripreso da dove avevo lasciato
                    pos=1;
                    delete this->stopAndResume;
                    this->stopAndResume=NULL;
                    
                    
                }else{
                    
                    //il corrente non e' piu' valido
                    //se e' la testa, allora significa che non ho nulla d leggere
                    if(this->current->next==NULL ||this->current==testa){
                        
                        if(this->stopAndResume!=NULL){
                            delete this->stopAndResume;
                            this->stopAndResume=NULL;
                            
                        }
                        this->stopAndResume=new SmartPointer<V >(this->current);
                        
                        return false;
                        
                    }
                    //ora devo cercare un elemento piu' nuovo del mio current attuale
                    //ho deciso di ripartire dal mio currenti, per cui devo incrementarlo
                    this->current->increment();
                    long limitTimeStamp=this->current->timedOut;
                    //se il pivot e' piu' vecchio del mio corrente, allora uso il mio corrente
                    
                    if(this->queue->pivot!=testa)  {
                        
                        
                        if(limitTimeStamp>this->queue->pivot->timedOut){
                            
                            if(!this->jumpToNextValidElement()){
                                
                                return false;
                            }
                            
                            //ho trovato un valore buono da leggere, cancello il vecchio smart pointer.
                            pos=1;
                            delete this->stopAndResume;
                            this->stopAndResume=NULL;
                        }else{
                            //il pivot e' piu nuovo del mio current per cuidevo usarlo
                            this->leaveStream();
                            
                            delete this->stopAndResume;
                            this->stopAndResume=NULL;
                            
                            this->queue->attachStream(this);
                            if(this->current->readerCount==0){
                                std::cout<<"male1";
                            }
                            
                            if(!this->jumpToNextValidElement()){
                                
                                return false;
                            }
                            if(this->current->readerCount==0){
                                std::cout<<"male2";
                            }
                            pos=2;
                            
                        }
                        
                    }else{
                        //il pivot e'  in testa, per cuidevo usarlo
                        this->leaveStream();
                        
                        delete this->stopAndResume;
                        this->stopAndResume=NULL;
                        
                        this->queue->attachStream(this);
                        if(!this->jumpToNextValidElement()){
                            
                            return false;
                        }
                        pos=3;
                        
                    }
                }
                
                this->stopAndResume=NULL;
                
            }else{
                if(this->current==NULL){
                    this->queue->attachStream(this);
                    pos=4;
                    
                }
            }
            
            return true;
        }
        void fallback(int count,std::vector<SmartPointer<V>* >* buffer){
            
            count++;
            for(int i=0;i<count;i++){
                SmartPointer<V>* temp= buffer->at(i);
                delete temp;
            }
            
            buffer->clear();
        }
        
        
        
       
        
    public:
        IteratorReader(CommonBuffer<V>* queue):IteratorBase<V>(queue){
            
            
        }
        
        
        
        /*!
         * It reads an amount of valid elements not readed.
         * It returns a vector of smart pointer to data.
         * Data pushed into buffer. Remeber to delete smart pointers.
         */
        int bufferedReader(int limit, std::vector<SmartPointer<V>* >* buffer){
            
            
            int count=0;
            if(limit<0)
                limit=INT_MAX;
            
            Element<AbstractDataElement<V> >* tempPivot=this->queue->lockPivot();
            
            //  do{
            if(!chooseCurrent()){
                this->queue->unlockPivot(tempPivot);
                
                return false;
            }
            //  }while(this->current->readerCount==0);
            
            this->queue->unlockPivot(tempPivot);
            
            Element<AbstractDataElement<V> >* testa=this->queue->head;
            
            
            while(count<limit && this->isReady() && this->current!=testa){
                
                
                // alloca un nuovo puntatore
                
                
                buffer->push_back(new SmartPointer<V>(this->current));
                /*
                 if(this->current->readerCount==0){
                 //forse non necessario
                 fallback(count,buffer);
                 if(this->stopAndResume!=NULL){
                 delete this->stopAndResume;
                 this->stopAndResume=NULL;
                 
                 }
                 this->current=NULL;
                 return 0;
                 
                 }*/
                
                this->updateIterator();
                
                count++;
                
                
            }
            
            if(this->stopAndResume!=NULL){
                delete this->stopAndResume;
                this->stopAndResume=NULL;
                
            }
            this->stopAndResume=new SmartPointer<V>(this->current);
            
            this->leaveStream();
            
            return count;
            
            
        }
        
    };
    
}
    
    
}
#endif
