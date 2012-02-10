//
//  SingleBufferCircularBuffer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

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
