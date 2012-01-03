    //
    //  CObjectProcessingQueueListener.h
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef CObjectProcessingQueueListener_H
#define CObjectProcessingQueueListener_H
/*
 Abstract class that implement some method for listen the execution on OBuffer element
 */
namespace chaos{
    template<class T>
    class CObjectProcessingQueueListener {
        
    public:
        /*
         Called befor the buffer will process the element, if this function
         return false, the process of this element wil be not executed, discardi
         the object
         */
        virtual bool elementWillBeProcessed(int bufferTag, T*)=0;
        virtual bool elementWillBeDiscarded(int bufferTag, T*)=0;
    };
}
#endif

