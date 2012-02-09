//
//  SignleBufferCircularBuffer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_SignleBufferCircularBuffer_h
#define CHAOSFramework_SignleBufferCircularBuffer_h

#include <chaos/common/utility/IDataBuffer.h>
namespace chaos {
    
    template<typename T>
    class SignleBufferCircularBuffer : public IDataBuffer<T> {
        
        //! the pointer to the memory for the buffer
        int64_t currentPostion;
        int64_t bufferDimension;
        T *basePointer; 
        T *writePointer;
    public:
        
        SignleBufferCircularBuffer(int64_t dim):bufferDimension(dim){
            basePointer = writePointer = (T*)malloc(dim*sizeof(T));
            currentPostion = 0;
        }
        
        ~SignleBufferCircularBuffer(){
            if(basePointer){
                free(basePointer);
            }
        }
        
        void addValue(T newValue) {
            (*(writePointer + currentPostion)) = newValue;
            currentPostion = currentPostion % bufferDimension;
        }
        
        inline int64_t getRiteBufferToEndDim(){
            return bufferDimension-currentPostion;
        }
        
        const T * const getBasePointer(){
            return basePointer;
        }
        
        const T * const getWritePointer(){
            return writePointer;
        }
    };
}

#endif
