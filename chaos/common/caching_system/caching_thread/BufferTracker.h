/*
 *	BufferTracker.h
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
#ifndef CachingSystem_BufferInformation_h
#define CachingSystem_BufferInformation_h

#include <boost/thread.hpp>
#include <string>
#include <chaos/common/caching_system/common_buffer/helper/IdFactory.h>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include <chaos/common/caching_system/common_buffer/IteratorReader.h>
#include <vector>

namespace chaos {
    namespace caching_system {
        namespace caching_thread{
            
            template <typename T>
            
            /*!
             * This class maintains information for a low resolution buffer. These buffer are istanced by tracker
             * when a clinet request a buffer of a different resolution with respect to principal buffer in highest resolution, which is mantained by the tracker directly. Informations are:
             * 1) a reference to the buffer
             * 2) The number of reader, this is compulsory for decide if a buffer must be deleted because
             *    is no longer used
             * 3) A reference to an iterator used by the garbage collector to delete old data
             * 4) The value of discretization, which if used to calculate the next minimum timestamp for insert a
             *     new value.
             */
            class BufferTracker{
                
            private:
                
                //!< This is a pointer to the low resolution buffer
                CommonBuffer<T>* controlledBuffer;
                //!< This is the actual numnber of readers
                uint32_t numberOfReader;
                
                //!< The value representig the discretization of this buffer
                uint64_t discretization;
                //!< pointer to iterator
                IteratorGarbage<T>* iteratorGarbage;
                std::vector<IteratorReader<T>* >* relatedIterators;
                //!< next timeout for new data
                uint64_t nextTimeout;
                
                
            public:
                
                /*!
                 * Creates the data structure to maintain data related to input buffer, you have to
                 * insert as input parameter:
                 * \param  CommonBuffer<T>* $controlledBuffer
                 * The low resolution buffer to be controlled
                 * \param uint64_t $discretization
                 * the value of the discretization
                 *
                 */
                BufferTracker(CommonBuffer<T>* controlledBuffer,uint64_t discretization){
                    this->nextTimeout=0;
                    this->controlledBuffer=controlledBuffer;
                    this->discretization=discretization;
                    iteratorGarbage=controlledBuffer->getIteratorGarbage();
                    iteratorGarbage->setId(caching_system::helper::getNewId());
                    
                    relatedIterators=new std::vector<IteratorReader<T>* >();
                    
                    
                }
                
                /*!
                 * It returns a new iterator for the controlled Buffer. This also increment the number
                 * of actual readers and gives to it a new Id
                 */
                IteratorReader<T>* getIterator(){
                    
                    //increments reader count atomically, so in this way we can prevent form locking
                    __sync_fetch_and_add(&this->numberOfReader,1);
                    
                    //request for new id
                    int id=caching_system::helper::getNewId();
                    
                    //create new iterator
                    IteratorReader<T>* ritorno= new IteratorReader<T>(controlledBuffer);
                    ritorno->setId(id);
                    relatedIterators->push_back(ritorno);
                    
                    return ritorno;
                    
                    
                }
                
                
                /*!
                 * Removes the iterator passed as argument by decrementing reader count and deleting it
                 */
                void deleteIterator(IteratorReader<T>* iterator){
                    
                    //decrement the number of reader and dealloc iterator
                    __sync_fetch_and_sub(&this->numberOfReader,1);
                    
                    //look for id, and remove it from vector, then delete iterator.
                    for(int i = 0;i<relatedIterators->size();i++){
                        
                        if(iterator->getId()==relatedIterators->at(i)->getId()){
                            
                            this->relatedIterators->erase(relatedIterators->begin()+i);
                        }
                    }
                    
                    
                    
                    delete iterator;
                }
                
                
                /*!
                 * Returns the iteratore used for garbaging
                 */
                IteratorGarbage<T>* getIteratorGarbage(){
                    
                    return this->iteratorGarbage;
                    
                }
                
                /*!
                 * Returns the actual number of readers. if it is zero, you can dealloc this buffer
                 */
                int getNumberOfReader(){
                    return this->numberOfReader;
                    
                }
                
                // removeIterator(IteretorReader<T>)
                
                
                /*!
                 * Returns the validity of this controlled buffer. It wraps the base function of the class CommonBuffer
                 */
                uint64_t getValidity(){
                    return this->controlledBuffer->getValidity();
                    
                    
                }
                
                /*!
                 * Returns the value of discretization managed for this buffer
                 */
                uint64_t inline getDiscretization(){
                    return this->discretization;
                }
                
                /*!
                 * Returns a pointer to the controlled buffer
                 */
                CommonBuffer<T>* getControlledBuffer(){
                    return this->controlledBuffer;
                }
                
                
                /*!
                 * Decides whether or not this buffer needs new data.
                 */
                bool needNextData(uint64_t timestamp){
                    if(timestamp>=this->nextTimeout){
                        this->nextTimeout=this->discretization+timestamp;
                        return true;
                    }
                    return false;
                    
                    
                }
                /*!
                 * This destructor deletes all related iterators, the controlled buffer and the iterator
                 * garbage of actual controlled buffer.
                 */
                ~BufferTracker(){
                    
                    for(int i=0;i<relatedIterators->size();i++){
                        deleteIterator(relatedIterators->at(i));
                    }
                    delete this->controlledBuffer;
                    delete this->iteratorGarbage;
                }
                
                
                
            };
            
            
        }
        
    }
}

#endif
