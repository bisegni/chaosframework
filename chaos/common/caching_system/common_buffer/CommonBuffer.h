/*
 *	CommonBuffer.h
 *	!CHAOS
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
#ifndef CachingSystem_CommonBuffer_h
#define CachingSystem_CommonBuffer_h


#include <iostream>
#include <chaos/common/caching_system/common_buffer/IteratorBase.h>
#include <chaos/common/caching_system/common_buffer/IteratorReader.h>

#include <chaos/common/caching_system/common_buffer/IteratorGarbage.h>
#include <chaos/common/caching_system/common_buffer/Element.h>
#include <boost/interprocess/detail/atomic.hpp>
#include <chaos/common/caching_system/common_buffer/MemoryAllocator.h>
#include <chaos/common/caching_system/common_buffer/data_element/AbstractDataElement.h>

namespace chaos {
    
    namespace caching_system {
        
        /*!
         * This class represents the queue. It takes pointers to the head,
         * tail and pivot. Iterators can be attached to this object. This queue is lock free,
         * and shuold be used in this way:
         * 1) Create ONLY ONE writer and write using enqueue()
         * 2) Create ONLY ONE Garbage Collector to delete unused elements
         * 3) Create N readers.
         * see also Iterator.h and Element.h
         * for examples on correct creation of these object and a more detailed description,
         * see full documentation on the website
         *
         */
        template<typename U>
        class CommonBuffer{
            friend class IteratorBase<U>;
            friend class IteratorReader<U>;
            friend class IteratorGarbage<U>;
            
        private:
            Element<AbstractDataElement<U> >* head;//!<  owned by the writer.
            Element<AbstractDataElement<U> >* tail;//!<  owned by Garbage collector.
            Element<AbstractDataElement<U> >* last;
            
            Element<AbstractDataElement<U> >* pivot;//!< owned by readers.
            
            uint64_t validita;
            
            
            boost::uint32_t elementiInCodaValidi;
            boost::uint32_t elementiInCodaValidiCorrenti;
            
            boost::uint32_t elementiInGarbage;
            boost::uint32_t elementiInCodaCorrenti;
            boost::uint32_t elementiInCoda;
            boost::uint32_t elementiInEliminati;
            double percentualeEliminati;
            boost::uint32_t numeroOsservazioni;
            
            
            MemoryAllocator< Element< AbstractDataElement<U> > >* allocator; //!< you can use custom memory allocator for this container.
            
            /*!
             * Allocs memory to a new node.
             * See also MemoryAllocator.h for more details.
             * \returns     a pointer to allocated space
             */
            Element<AbstractDataElement<U> >* allocNode(){
                
                //for statistics. delete in production
                incrementElementCounter();
                
                return allocator->allocate();
                
            }
            
            /*!
             * Deallocs memory for an unused node
             * \param[in] node      pointer to a node
             */
            void deAllocNode(Element<AbstractDataElement<U> >* node){
                //for statistics. delete in production
                decrementElementCounter();
                
                allocator->deallocate(node);
            }
            
            
            /*!
             * It increments elements counter atomically
             */
            void inline incrementElementCounter(){
                boost::interprocess::ipcdetail::atomic_inc32(&elementiInCodaCorrenti);
                
            }
            
            void inline decrementElementCounter(){
                boost::interprocess::ipcdetail::atomic_dec32(&elementiInCodaCorrenti);
                
            }
            
            Element<AbstractDataElement<U> >* lockPivot(){
                Element<AbstractDataElement<U> >* inizio=NULL;
                Element<AbstractDataElement<U> >* dopo=NULL;
                do{
                    inizio= this->pivot;
                    inizio->increment();
                    
                    
                    dopo=this->pivot;
                    
                    if(inizio==dopo){
                        break;
                    }else{
                        inizio->decrementCounter();
                    }
                    
                }while(true);
                
                
                
                return inizio;
                
                
            }
            
            
            
            
            void unlockPivot(Element<AbstractDataElement<U> >* pivo){
                pivo->decrementCounter();
                
            }
            /*!
             * It clears unused memory. Elements with reference count =0 will be destroyed.
             * if you have a reference count >0 (that is referenced by a smart pointer) on an
             * element, it won't be destroyed.
             */
            void clearUnusedData(){
                
                Element<AbstractDataElement<U> >* cursor=tail;
                Element<AbstractDataElement<U> >* end=pivot;
                Element<AbstractDataElement<U> >* prev=NULL;
                Element<AbstractDataElement<U> >* testa=head;
                Element<AbstractDataElement<U> >* succ=NULL;
                
                int elementiTotaliGarbage=0;
                int elementiEliminati=0;
                
                
                //until you have check all these values
                while(cursor!=end&&cursor!=testa/*&&cursor->next!=end*/){
                    if(cursor==testa||cursor==NULL)
                        break;
                    //if reference count is zero
                    if(cursor->referenceCount<=0){
                        //if it is the tail, cut it
                        if(cursor==tail){
                            dequeue();
                            
                            cursor=tail;
                            prev=NULL;
                        }else{
                            //if it is another element
                            //release it from queue
                            succ=cursor->next;
                            compareAndSwap(&prev->next,prev->next,cursor->next);
                            
                            // prev->next=cursor->next;
                            //destroy removed node
                            deAllocNode(cursor);
                            //and go on
                            cursor=succ;
                            
                            if(cursor==NULL){
                                std::cout<<"questo non doveva succedere\n";
                                break;
                            }
                            
                        }
                        
                        elementiEliminati++;
                    }else{
                        //go to the next
                        prev=cursor;
                        cursor=cursor->next;
                        //if cursor is null, i have reached the head, so stop
                        if(cursor==NULL)
                            break;
                        
                        
                    }
                    elementiTotaliGarbage++;
                    
                }
                
                //statistiche
                
                elementiInGarbage=elementiInGarbage + (elementiTotaliGarbage);
                elementiInCoda = elementiInCoda + elementiInCodaCorrenti;
                elementiInCodaValidiCorrenti=(elementiInCodaCorrenti-elementiTotaliGarbage);
                elementiInCodaValidi=elementiInCodaValidi+elementiInCodaValidiCorrenti;
                elementiInEliminati=elementiInEliminati+elementiEliminati;
                
                //    percentualeEliminati=percentualeEliminati+((double)elementiEliminati)/((double)elementiInCoda);
                
                numeroOsservazioni++;
                
            }
            
            
            public :
            
            
            
            /*!
             * Initialize the queue with a dummy element
             */
            CommonBuffer(uint64_t validity){
                
                head=new Element<AbstractDataElement<U> >();
                tail=head;
                pivot=head;
                //numElement=0;
                last=NULL;
                //stats
                elementiInCoda=0;
                elementiInCodaCorrenti=0;
                elementiInCodaValidi=0;
                elementiInGarbage=0;
                numeroOsservazioni=0;
                elementiInEliminati=0;
                elementiInCodaValidiCorrenti=0;
                percentualeEliminati=0;
                if(validity<1)
                    validity=1;
                this->validita=validity;
                
                allocator=new MemoryAllocator<Element<AbstractDataElement<U> > >();
                
                
                
            }
            
            /*!
             * Destroy queue and clean used memory.
             */
            ~CommonBuffer(){
                Element<AbstractDataElement<U> >* toDelete;
                Element<AbstractDataElement<U> >* next;
                toDelete=tail;
                
                while(toDelete->next!=NULL){
                    
                    next=toDelete->next;
                    allocator->deallocate( toDelete);
                    toDelete=next;
                    
                }
                
                allocator->deallocate( head);
                delete allocator;
                
            }
            
            /*!
             * Returns a new iterator linked to the last valid element (the actual pivot).
             * Pay attention: getting the iterator prevent to shift pivot ahead. Use only
             * when you are going to read. When you finished to read, remember to delete iterator or
             * detach it.
             * If you read using bufferedReader(), on IteratorReader, bufferedReader() controls it for
             * you, and only one element will remain in memory, so do not use this for reading.
             * It is helpful for getting an iterator by garbage collector.
             *
             */
            IteratorGarbage<U>* getIteratorGarbage(){
                
                //no one can shift pivot except me
                this->pivot->increment();
                
                //now i set pivot in new iteretor and i give it back
                IteratorGarbage<U>* iterator=new IteratorGarbage<U>(this, this->pivot);
                return iterator;
                
                
                
            }
            
            /*!
             * It links an iterator to the queue
             */
            void inline attachStream(IteratorBase<U>* iterator){
                Element<AbstractDataElement<U> >* inizio=NULL;
                Element<AbstractDataElement<U> >* dopo=NULL;
                do{
                    inizio= this->pivot;
                    inizio->increment();
                    
                    
                    dopo=this->pivot;
                    
                    if(inizio==dopo){
                        break;
                        
                    }else{
                        inizio->decrementCounter();
                    }
                    
                }while(true);
                
                iterator->setPivot(inizio);
                
                
            }
            
            /*!
             * It unlink an iterator from queue. You can do this also on the iterator directly.
             */
            void inline detachStream(IteratorBase<U>* iterator){
                iterator->leaveStream();
            }
            
            /*!
             * This function is called by Garbage collector. Deletes elements on the tail.
             * It returns true if it can delete the tail.
             * False otherwise.
             */
            bool dequeue(){
                Element<AbstractDataElement<U> >* temp=tail;
                if(temp==pivot||temp==head/*||temp->next==pivot*/){
                    return false;
                }
                //cambio la coda e dealloco il nodo
                compareAndSwap(&tail,temp,temp->next);
                deAllocNode(temp);
                return true;
            }
            
            /*!
             *
             */
            SmartPointer< U >* getLast(){
                
                //avoiding to delete any elment
                this->lockPivot();
                //getting last element
                
                Element<AbstractDataElement<U> >* lastElement=this->last;
                //create a smart pointer to last element
                SmartPointer<U>* smartPointerLast=new SmartPointer<U>(lastElement);
                //restore pivot
                this->unlockPivot();
                return smartPointerLast;
            }
            
            
            
            /*!
             * Writer must be unique. Do not expect concurrence on this procedure
             * Garbage collector works until pivot, and readers read from pivot to the head (except
             * head)
             */
            SmartPointer< U >* enqueue(AbstractDataElement<U>* data,bool smartPointer){
                //do work
                Element<AbstractDataElement<U> >* node=allocNode();
                Element<AbstractDataElement<U> >* ritorno=NULL;
                node->next=NULL;
                Element<AbstractDataElement<U> >* testa=head;
                SmartPointer<U>* rit=NULL;
                long timeout=data->getTimestamp()+this->validita;
                testa->setData(data,timeout);
                ritorno=testa;
                //mantaining the last element inserted
                last=testa;
                if(smartPointer){
                    rit= new SmartPointer<U>(ritorno);
                    //return rit;
                }
                //then publish
                this->compareAndSwap(&testa->next,testa->next,node);
                
                this->compareAndSwap(&this->head,testa,testa->next);
                
                return rit;
                
                
            }
            
            void setValidity(uint64_t validity){
                if(validity<1)
                    validity=1;
                this->validita=validity;
            }
            
            uint64_t getValidity(){
                return validita;
            }
            
            int inline getCounter(){
                return  elementiInCodaCorrenti;
            }
            
            int inline getElementInValidity(){
                return elementiInCodaValidiCorrenti;
            }
            
            /*
             * Funzioni di calcolo per le statitstiche
             */
            
            double inline getNumeroMedioElementiInGc(){
                double media;
                media = ((double)elementiInGarbage)/((double)numeroOsservazioni);
                return media;
                
            }
            
            
            double inline getNumeroMedioElementiInCodaValidi(){
                double media;
                media = ((double)elementiInCodaValidi)/((double)numeroOsservazioni);
                return media;
                
            }
            
            double inline getNumeroMedioElementiInCodaTotali(){
                double media;
                media = ((double)elementiInCoda)/((double)numeroOsservazioni);
                return media;
                
            }
            
            double inline getNumeroMedioElementiInCodaEliminati(){
                double media;
                media = ((double)elementiInEliminati)/((double)numeroOsservazioni);
                return media;
            }
            
            double inline getNumeroMedioElementiEliminati(){
                double media;
                media = ((double)percentualeEliminati)/*/((double)numeroOsservazioni)*/;
                return media;
            }
            
            int inline getOsservazioni(){
                return numeroOsservazioni;
            }
            
            
            bool inline compareAndSwap(Element<AbstractDataElement<U> >** mem,Element<AbstractDataElement<U> >* with , Element<AbstractDataElement<U> >* ex){
                return __sync_bool_compare_and_swap(mem,with,ex);
            }
            
            
            
            
        };
        
    }
}

#endif
