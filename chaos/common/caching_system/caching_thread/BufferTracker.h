/*
 *	BufferTracker.h
 *	!CHOAS
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

namespace chaos {
    namespace caching_system {
        namespace caching_thread{
            // static int idReader=0;
            
            template <typename T>
            class BufferTracker{
                
            private:
                
                CommonBuffer<T>* controlledBuffer;
                uint32_t numberOfReader;
                uint64_t discretization;
                IteratorGarbage<T>* iteratorGarbage;
                uint64_t nextTimeout;
                //std::string keyId;
                
                
            public:
                
                BufferTracker(CommonBuffer<T>* controlledBuffer,uint64_t discretization){
                    this->nextTimeout=0;
                    this->controlledBuffer=controlledBuffer;
                    this->discretization=discretization;
                    iteratorGarbage=controlledBuffer->getIteratorGarbage();
                    iteratorGarbage->setId(caching_system::helper::getNewId());
                    
                }
                
                IteratorReader<T>* getIterator(){
                    //increments reader count atomically
                    __sync_fetch_and_add(&this->numberOfReader,1);
                    
                    //request for new id
                    int id=caching_system::helper::getNewId();
                    
                    //create new iterator
                    IteratorReader<T>* ritorno= new IteratorReader<T>(controlledBuffer);
                    ritorno->setId(id);
                    return ritorno;
                    
                    
                }
                
                void deleteIterator(IteratorReader<T>* iterator){
                    
                    __sync_fetch_and_sub(&this->numberOfReader,1);
                    
                    delete iterator;
                }
                
                IteratorGarbage<T>* getIteratorGarbage(){
                    
                    return this->iteratorGarbage;
                    
                }
                
                int getNumberOfReader(){
                    return this->numberOfReader;
                    
                }
                
                // removeIterator(IteretorReader<T>)
                
                
                
                uint64_t getValidity(){
                    return this->controlledBuffer->getValidity();
                    
                    
                }
                
                uint64_t inline getDiscretization(){
                    return this->discretization;
                }
                
                CommonBuffer<T>* getControlledBuffer(){
                    return this->controlledBuffer;
                }
                
                bool needNextData(uint64_t timestamp){
                    if(timestamp>=this->nextTimeout){
                        this->nextTimeout=this->discretization+timestamp;
                        return true;
                    }
                    return false;
                    
                    
                }
                
                //  void insertedNewData(long timestamp){
                
                
                
                //   }
                
                ~BufferTracker(){
                    delete this->controlledBuffer;
                    delete this->iteratorGarbage;
                }
                
                
                
            };
            
            
        }
        
    }
}

#endif
