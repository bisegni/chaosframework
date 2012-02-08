//
//  SignleBufferCircularBuffer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_SignleBufferCircularBuffer_h
#define CHAOSFramework_SignleBufferCircularBuffer_h

namespace chaos {
    
    template<typename T>
    class SignleBufferCircularBuffer {
        
        //! the pointer to the memory for the buffer
        T *baseBufferPointer; 
        T *writeNextPointer;
    public:
        
    };
}

#endif
