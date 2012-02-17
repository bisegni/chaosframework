/*	
 *	 SingleBufferCircularBuffer.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
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

#ifndef CHAOSFramework_SingleBufferCircularBuffer_h
#define CHAOSFramework_SingleBufferCircularBuffer_h

#include <cstring>

namespace chaos {
    
    class DataBuffer {
    public:
        virtual int64_t getWriteBufferPosition() = 0;
        
        virtual int64_t getDimension() = 0;
        
        virtual void * const getBasePointer() = 0;
        
        virtual void * const getWritePointer() = 0;
    };
    
    template<typename T>
    class SingleBufferCircularBuffer : public DataBuffer {
        
        //! the pointer to the memory for the buffer
        int64_t currentPostion;
        int64_t bufferDimension;
        T *basePointer; 
        T *writePointer;
    public:
        
        SingleBufferCircularBuffer(int64_t dim):bufferDimension(dim){
            basePointer = writePointer = (T*)malloc(dim*sizeof(T));
            std::memset(basePointer, 0, dim*sizeof(T));
            currentPostion = 0;
        }
        
        ~SingleBufferCircularBuffer(){
            if(basePointer){
                free(basePointer);
            }
        }
        
        void addValue(T newValue) {
            (*writePointer) = newValue;
            currentPostion = (++currentPostion) % bufferDimension;
            writePointer = basePointer + currentPostion;
        }
        
        int64_t getWriteBufferPosition(){
            return currentPostion;
        }
        
        int64_t getDimension(){
            return bufferDimension;
        }
        
        void * const getBasePointer(){
            return basePointer;
        }
        
        void * const getWritePointer(){
            return writePointer;
        }
    };
}
#endif
